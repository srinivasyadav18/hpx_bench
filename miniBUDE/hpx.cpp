#include <cstdint>
#include <string>
#include <iomanip>
#include <limits>
#include <cmath>
#include <memory>
#include <vector>
#include <chrono>
#include <iostream>
#include <fstream>
#include <functional>
#include <algorithm>


#include <hpx/hpx.hpp>
#include <hpx/hpx_main.hpp>
#include <hpx/parallel/algorithms/for_loop.hpp>

#include "bude.h"

struct __attribute__((__packed__)) Atom {
	float x, y, z;
	int32_t type;
};

struct __attribute__((__packed__)) FFParams {
	int32_t hbtype;
	float radius;
	float hphb;
	float elsc;
};


#define ZERO    0.0f
#define QUARTER 0.25f
#define HALF    0.5f
#define ONE     1.0f
#define TWO     2.0f
#define FOUR    4.0f
#define CNSTNT 45.0f

// Energy evaluation parameters
#define HBTYPE_F 70
#define HBTYPE_E 69
#define HARDNESS 38.0f
#define NPNPDIST 5.5f
#define NPPDIST  1.0f

constexpr const auto FloatMax = std::numeric_limits<float>::max();

void fasten_main(
		size_t ntypes, size_t nposes,
		size_t natlig, size_t natpro,
		std::vector<Atom> &protein_molecule,
		std::vector<Atom> &ligand_molecule,
		std::vector<float> &transforms_0,
		std::vector<float> &transforms_1,
		std::vector<float> &transforms_2,
		std::vector<float> &transforms_3,
		std::vector<float> &transforms_4,
		std::vector<float> &transforms_5,
		std::vector<FFParams> &forcefield,
		std::vector<float> &etotals) 
{
	int n_groups = nposes / WG_SIZE;

	hpx::experimental::for_loop(hpx::execution::par, 0, n_groups, 
	[&](const size_t group) {
		float etot[WG_SIZE];
		float transform[3][4][WG_SIZE];


		// Compute transformation matrix to private memory
		for (size_t i = 0; i < WG_SIZE; i++) {

			size_t ix = group * WG_SIZE + i;

			const float sx = sinf(transforms_0[ix]);
			const float cx = cosf(transforms_0[ix]);
			const float sy = sinf(transforms_1[ix]);
			const float cy = cosf(transforms_1[ix]);
			const float sz = sinf(transforms_2[ix]);
			const float cz = cosf(transforms_2[ix]);

			transform[0][0][i] = cy * cz;
			transform[0][1][i] = sx * sy * cz - cx * sz;
			transform[0][2][i] = cx * sy * cz + sx * sz;
			transform[0][3][i] = transforms_3[ix];
			transform[1][0][i] = cy * sz;
			transform[1][1][i] = sx * sy * sz + cx * cz;
			transform[1][2][i] = cx * sy * sz - sx * cz;
			transform[1][3][i] = transforms_4[ix];
			transform[2][0][i] = -sy;
			transform[2][1][i] = sx * cy;
			transform[2][2][i] = cx * cy;
			transform[2][3][i] = transforms_5[ix];

			etot[i] = ZERO;
		}

		// Loop over ligand atoms
		size_t il = 0;
		do {
			// Load ligand atom data
			const Atom l_atom = ligand_molecule[il];
			const FFParams l_params = forcefield[l_atom.type];
			const bool lhphb_ltz = l_params.hphb < ZERO;
			const bool lhphb_gtz = l_params.hphb > ZERO;

			float lpos_x[WG_SIZE], lpos_y[WG_SIZE], lpos_z[WG_SIZE];
			for (size_t i = 0; i < WG_SIZE; i++) {
				// Transform ligand atom
				lpos_x[i] = transform[0][3][i]
						+ l_atom.x * transform[0][0][i]
						+ l_atom.y * transform[0][1][i]
						+ l_atom.z * transform[0][2][i];
				lpos_y[i] = transform[1][3][i]
						+ l_atom.x * transform[1][0][i]
						+ l_atom.y * transform[1][1][i]
						+ l_atom.z * transform[1][2][i];
				lpos_z[i] = transform[2][3][i]
						+ l_atom.x * transform[2][0][i]
						+ l_atom.y * transform[2][1][i]
						+ l_atom.z * transform[2][2][i];
			}

			// Loop over protein atoms
			size_t ip = 0;
			do {
				// Load protein atom data
				const Atom p_atom = protein_molecule[ip];
				const FFParams p_params = forcefield[p_atom.type];

				const float radij = p_params.radius + l_params.radius;
				const float r_radij = 1.f / (radij);

				const float elcdst = (p_params.hbtype == HBTYPE_F && l_params.hbtype == HBTYPE_F) ? FOUR : TWO;
				const float elcdst1 = (p_params.hbtype == HBTYPE_F && l_params.hbtype == HBTYPE_F) ? QUARTER : HALF;
				const bool type_E = ((p_params.hbtype == HBTYPE_E || l_params.hbtype == HBTYPE_E));

				const bool phphb_ltz = p_params.hphb < ZERO;
				const bool phphb_gtz = p_params.hphb > ZERO;
				const bool phphb_nz = p_params.hphb != ZERO;
				const float p_hphb = p_params.hphb * (phphb_ltz && lhphb_gtz ? -ONE : ONE);
				const float l_hphb = l_params.hphb * (phphb_gtz && lhphb_ltz ? -ONE : ONE);
				const float distdslv = (phphb_ltz ? (lhphb_ltz ? NPNPDIST : NPPDIST) : (lhphb_ltz ? NPPDIST : -FloatMax));
				const float r_distdslv = 1.f / (distdslv);

				const float chrg_init = l_params.elsc * p_params.elsc;
				const float dslv_init = p_hphb + l_hphb;

				for (size_t i = 0; i < WG_SIZE; i++) {
					// Calculate distance between atoms
					const float x = lpos_x[i] - p_atom.x;
					const float y = lpos_y[i] - p_atom.y;
					const float z = lpos_z[i] - p_atom.z;

					const float distij = sqrtf(x * x + y * y + z * z);

					// Calculate the sum of the sphere radii
					const float distbb = distij - radij;
					const bool zone1 = (distbb < ZERO);

					// Calculate steric energy
					etot[i] += (ONE - (distij * r_radij)) * (zone1 ? 2 * HARDNESS : ZERO);

					// Calculate formal and dipole charge interactions
					float chrg_e = chrg_init * ((zone1 ? 1 : (ONE - distbb * elcdst1)) * (distbb < elcdst ? 1 : ZERO));
					const float neg_chrg_e = -fabsf(chrg_e);
					chrg_e = type_E ? neg_chrg_e : chrg_e;
					etot[i] += chrg_e * CNSTNT;

					// Calculate the two cases for Nonpolar-Polar repulsive interactions
					const float coeff = (ONE - (distbb * r_distdslv));
					float dslv_e = dslv_init * ((distbb < distdslv && phphb_nz) ? 1 : ZERO);
					dslv_e *= (zone1 ? 1 : coeff);
					etot[i] += dslv_e;
				}
			} while (++ip < natpro); // loop over protein atoms
		} while (++il < natlig); // loop over ligand atoms

		// Write results
		for (size_t l = 0; l < WG_SIZE; l++) {
			// Write result
			etotals[group * WG_SIZE + l] = etot[l] * 0.5f;
		}
	});

}

typedef std::chrono::high_resolution_clock::time_point TimePoint;

struct Params {

	size_t natlig;
	size_t natpro;
	size_t ntypes;
	size_t nposes;

	std::vector<Atom> protein;
	std::vector<Atom> ligand;
	std::vector<FFParams> forcefield;
	std::array<std::vector<float>, 6> poses;

	size_t iterations;

	std::string deckDir;

	friend std::ostream &operator<<(std::ostream &os, const Params &params) {
		os <<
				"natlig:      " << params.natlig << "\n" <<
				"natpro:      " << params.natpro << "\n" <<
				"ntypes:      " << params.ntypes << "\n" <<
				"nposes:      " << params.nposes << "\n" <<
				"iterations:  " << params.iterations << "\n" <<
				"wgSize:      " << WG_SIZE << "\n";
		return os;
	}
};

double elapsedMillis( const TimePoint &start, const TimePoint &end){
	auto elapsedNs = static_cast<double>(
			std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());
	return elapsedNs * 1e-6;
}

void printTimings(const Params &params, double millis) {

	// Average time per iteration
	double ms = (millis / params.iterations);
	double runtime = ms * 1e-3;

	// Compute FLOP/s
	double ops_per_wg = WG_SIZE * 27 +
			params.natlig * (2 +
					WG_SIZE * 18 +
					params.natpro * (10 + WG_SIZE * 30)
			) + WG_SIZE;
	double total_ops = ops_per_wg * ((double) params.nposes / WG_SIZE);
	double flops = total_ops / runtime;
	double gflops = flops / 1e9;

	double total_finsts = 25.0 * params.natpro * params.natlig * params.nposes;
	double finsts = total_finsts / runtime;
	double gfinsts = finsts / 1e9;

	double interactions = (double) params.nposes * (double) params.natlig * (double) params.natpro;
	double interactions_per_sec = interactions / runtime;

	// Print stats
	std::cout.precision(3);
	std::cout << std::fixed;
	std::cout << "- Kernel time:    " << (millis) << " ms\n";
	std::cout << "- Average time:   " << ms << " ms\n";
	std::cout << "- Interactions/s: " << (interactions_per_sec / 1e9) << " billion\n";
	std::cout << "- GFLOP/s:        " << gflops << "\n";
	std::cout << "- GFInst/s:       " << gfinsts << "\n";
}

template<typename T>
std::vector<T> readNStruct(const std::string &path) {
	std::fstream s(path, std::ios::binary | std::ios::in);
	if (!s.good()) {
		throw std::invalid_argument("Bad file: " + path);
	}
	s.ignore(std::numeric_limits<std::streamsize>::max());
	auto len = s.gcount();
	s.clear();
	s.seekg(0, std::ios::beg);
	std::vector<T> xs(len / sizeof(T));
	s.read(reinterpret_cast<char *>(xs.data()), len);
	s.close();
	return xs;
}


Params loadParameters(const std::vector<std::string> &args) {

	Params params = {};

	// Defaults
	params.iterations = DEFAULT_ITERS;
	params.nposes = DEFAULT_NPOSES;
	params.deckDir = DATA_DIR;

	const auto readParam = [&args](size_t &current,
			const std::string &arg,
			const std::initializer_list<std::string> &matches,
			const std::function<void(std::string)> &handle) {
		if (matches.size() == 0) return false;
		if (std::find(matches.begin(), matches.end(), arg) != matches.end()) {
			if (current + 1 < args.size()) {
				current++;
				handle(args[current]);
			} else {
				std::cerr << "[";
				for (const auto &m : matches) std::cerr << m;
				std::cerr << "] specified but no value was given" << std::endl;
				std::exit(EXIT_FAILURE);
			}
			return true;
		}
		return false;
	};

	const auto bindInt = [](const std::string &param, size_t &dest, const std::string &name) {
		try {
			auto parsed = std::stol(param);
			if (parsed < 0) {
				std::cerr << "positive integer required for <" << name << ">: `" << parsed << "`" << std::endl;
				std::exit(EXIT_FAILURE);
			}
			dest = parsed;
		} catch (...) {
			std::cerr << "malformed value, integer required for <" << name << ">: `" << param << "`" << std::endl;
			std::exit(EXIT_FAILURE);
		}
	};


	for (size_t i = 0; i < args.size(); ++i) {
		using namespace std::placeholders;
		const auto arg = args[i];
		if (readParam(i, arg, {"--iterations", "-i"}, std::bind(bindInt, _1, std::ref(params.iterations), "iterations"))) continue;
		if (readParam(i, arg, {"--numposes", "-n"}, std::bind(bindInt, _1, std::ref(params.nposes), "numposes"))) continue;
		if (readParam(i, arg, {"--deck"}, [&](const std::string &param) { params.deckDir = param; })) continue;

		if (arg == "--help" || arg == "-h") {
			std::cout << "\n";
			std::cout << "Usage: ./bude [OPTIONS]\n\n"
					<< "Options:\n"
					<< "  -h  --help               Print this message\n"
					<< "  -i  --iterations I       Repeat kernel I times (default: " << DEFAULT_ITERS << ")\n"
					<< "  -n  --numposes   N       Compute energies for N poses (default: " << DEFAULT_NPOSES << ")\n"
					<< "      --deck       DECK    Use the DECK directory as input deck (default: " << DATA_DIR << ")"
					<< std::endl;
			std::exit(EXIT_SUCCESS);
		}

		std::cout << "Unrecognized argument '" << arg << "' (try '--help')" << std::endl;
		std::exit(EXIT_FAILURE);
	}

	params.ligand = readNStruct<Atom>(params.deckDir + FILE_LIGAND);
	params.natlig = params.ligand.size();

	params.protein = readNStruct<Atom>(params.deckDir + FILE_PROTEIN);
	params.natpro = params.protein.size();

	params.forcefield = readNStruct<FFParams>(params.deckDir + FILE_FORCEFIELD);
	params.ntypes = params.forcefield.size();

	auto poses = readNStruct<float>(params.deckDir + FILE_POSES);
	if (poses.size() / 6 != params.nposes) {
		throw std::invalid_argument("Bad poses: " + std::to_string(poses.size()));
	}

	for (size_t i = 0; i < 6; ++i) {
		params.poses[i].resize(params.nposes);
		std::copy(
				std::next(poses.cbegin(), i * params.nposes),
				std::next(poses.cbegin(), i * params.nposes + params.nposes),
				params.poses[i].begin());

	}

	return params;
}

std::vector<float> runKernel(Params params) {

	std::vector<float> energies(params.nposes);

	auto protein = params.protein;
	auto ligand = params.ligand;
	auto transforms_0 = params.poses[0];
	auto transforms_1 = params.poses[1];
	auto transforms_2 = params.poses[2];
	auto transforms_3 = params.poses[3];
	auto transforms_4 = params.poses[4];
	auto transforms_5 = params.poses[5];
	auto forcefield = params.forcefield;

	const auto runKernel = [&]() {
		fasten_main(
				params.ntypes, params.nposes,
				params.natlig, params.natpro,
				protein, ligand,
				transforms_0, transforms_1, transforms_2,
				transforms_3, transforms_4, transforms_5,
				forcefield, energies);
	};

	// warm up
	runKernel();

	auto start = std::chrono::high_resolution_clock::now();
	for (size_t i = 0; i < params.iterations; ++i) {
		runKernel();
	}
	auto end = std::chrono::high_resolution_clock::now();

	printTimings(params, elapsedMillis(start, end));
	return energies;
}

int main(int argc, char *argv[]) {


	auto args = std::vector<std::string>(argv + 1, argv + argc);
	auto params = loadParameters(args);

	std::cout << "backend   : hpx" << std::endl;
	std::cout << "# thread  : " << hpx::get_num_worker_threads() << std::endl;
	std::cout << "Poses     : " << params.nposes << std::endl;
	std::cout << "Iterations: " << params.iterations << std::endl;
	std::cout << "Ligands   : " << params.natlig << std::endl;
	std::cout << "Proteins  : " << params.natpro << std::endl;
	std::cout << "Deck      : " << params.deckDir << std::endl;
	std::cout << "WG_SIZE   : " << WG_SIZE << std::endl;

	auto energies = runKernel(params);

	//XXX Keep the output format consistent with the C impl. so no fancy streams here
	FILE *output = fopen("energies.out", "w+");
	// Print some energies
	printf("\nEnergies\n");
	for (size_t i = 0; i < params.nposes; i++) {
		fprintf(output, "%7.2f\n", energies[i]);
		if (i < 16)
			printf("%7.2f\n", energies[i]);
	}

	// Validate energies
	std::ifstream refEnergies(params.deckDir + FILE_REF_ENERGIES);
	size_t nRefPoses = params.nposes;
	if (params.nposes > REF_NPOSES) {
		std::cout << "Only validating the first " << REF_NPOSES << " poses.\n";
		nRefPoses = REF_NPOSES;
	}

	std::string line;
	float maxdiff = 0.0f;
	for (size_t i = 0; i < nRefPoses; i++) {
		if (!std::getline(refEnergies, line)) {
			throw std::logic_error("ran out of ref energies lines to verify");
		}
		float e = std::stof(line);
		if (std::fabs(e) < 1.f && std::fabs(energies[i]) < 1.f) continue;


		float diff = std::fabs(e - energies[i]) / e;
//		std::cout <<  "" << i << " = "<< diff << " " << "\n";

		if (diff > maxdiff) maxdiff = diff;
	}
	std::cout << "Largest difference was " <<
			std::setprecision(3) << (100 * maxdiff)
			<< "%.\n\n"; // Expect numbers to be accurate to 2 decimal places
	refEnergies.close();

	fclose(output);
}
