#include <iostream>
#include <vector>
#include <cmath>
#include <sstream>
#include <fstream>

struct Vector {
    std::vector<double> components;
};

// Parsing vectors logic
std::vector<std::vector<double>> parseVectors(const std::string& input) {
    std::vector<std::vector<double>> result;
    std::istringstream iss(input);

    char bracket;
    double component;

    // Process each character in the input string
    while (iss >> bracket >> component) {
        if (bracket == '[') {
            Vector vector;
            vector.components.push_back(component);

            // Process the components inside the brackets
            while (iss >> component) {
                vector.components.push_back(component);
                // Check for a closing bracket
                if (iss >> std::ws && iss.peek() == ']') {
                    iss.ignore();
                    break;
                }
            }
            result.push_back(vector.components);
        } else if (bracket != ' ') {
            std::cerr << "Error: Invalid vector format." << std::endl;
            exit(1);
        }
    }

    return result;
}

// Working out the Euclidean norm & length
double euclidean_norm(const Vector& v) {
    double sum = 0;
    for (double component : v.components) {
        sum += component * component;
    }
    return sqrt(sum);
}

double euclideanLength(const Vector& v, int dim) {
    double length = 0.0;
    for (int i = 0; i < dim; ++i) {
        length += v.components[i] * v.components[i];
    }
    return sqrt(length);
}

// The dot product function
double dotProduct(const Vector& v1, const Vector& v2, int dim) {

    double result = 0.0;
    for (int i = 0; i < dim; ++i) {
        result += v1.components[i] * v2.components[i];
    }
    return result;
}

// Gram Schmidt process
void gramSchmidt(Vector* basis, int dim) {
    for (int i = 0; i < dim; i++) {
        for (int j = 0; j < i; j++) {
            const double coeff = std::lround(dotProduct(basis[i], basis[j], dim) / dotProduct(basis[j], basis[j], dim));
            for (int k = 0; k < dim; k++) {
                basis[i].components[k] -= coeff * basis[j].components[k];
            }
        }
    }
}

// LLL reduction on basis vectors
void lllReduction(Vector* basis, int dim) {
    const int maxIterations = 100;
    for (int iter = 0; iter < maxIterations; iter++) {
        for (int i = 0; i < dim; i++) {
            for (int j = i - 1; j >= 0; j--) {
                const double coeff = round(dotProduct(basis[i], basis[j], dim) / dotProduct(basis[j], basis[j], dim));
                for (int k = 0; k < dim; k++) {
                    basis[i].components[k] -= coeff * basis[j].components[k];
                }
            }
        }
        gramSchmidt(basis, dim);
    }
}

// Post Processing to orthogonalise a set of vectors in a vector space
void postProcessBasis(Vector* basis, int dim) {
    const int maxIterations = 100;
    for (int iter = 0; iter < maxIterations; iter++) {
        bool madeProgress = false;
        for (int i = 0; i < dim; i++) {
            for (int j = 0; j < dim; j++) {
                if (i != j) {
                    const double coeff = round(dotProduct(basis[i], basis[j], dim) / dotProduct(basis[j], basis[j], dim));
                    if (coeff != 0.0) {
                        const double originalLength = euclideanLength(basis[i], dim);
                        for (int k = 0; k < dim; k++) {
                            basis[i].components[k] -= coeff * basis[j].components[k];
                        }
                        const double newLength = euclideanLength(basis[i], dim);
                        if (newLength < originalLength) {
                            madeProgress = true;
                        } else {
                            // Undo the reduction if it makes zero difference
                            for (int k = 0; k < dim; k++) {
                                basis[i].components[k] += coeff * basis[j].components[k];
                            }
                        }
                    }
                }
            }
        }
        if (!madeProgress) {
            break;
        }
    }
}

// Bruteforce method to work out the shortest vector
Vector shortest_vector_bruteforce(const std::vector<Vector>& vectors) {
    Vector shortest_vector;
    double min_norm = std::numeric_limits<double>::max();

    for (const auto& vector : vectors) {
        double norm = euclidean_norm(vector);
        if (norm > 0 && norm < min_norm) {
            min_norm = norm;
            shortest_vector = vector;
        }
    }

    return shortest_vector;
}


int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " [a b c] [d e f] [g h i] ..." << std::endl;
        return 1;
    }

    // Combine command-line arguments into a single string
    std::string input;
    for (int i = 1; i < argc; ++i) {
        input += argv[i];
        if (i < argc - 1) {
            input += " ";
        }
    }

    // Parse the combined string into vectors
    std::vector<std::vector<double>> vectors = parseVectors(input);

    // Display the parsed vectors
    std::cout << "Parsed Vectors:" << std::endl;
    for (const auto& vector : vectors) {
        std::cout << "{";
        for (double component : vector) {
            std::cout << component << " ";
        }
        std::cout << "}" << std::endl;
    }



    // Post-process the basis vectors for further refinement
    std::vector<Vector> basis_vectors;
    for (const auto& vector : vectors) {
        basis_vectors.push_back({vector});
    }
    postProcessBasis(basis_vectors.data(), basis_vectors.size());

    // LLL reduction
    lllReduction(basis_vectors.data(), basis_vectors.size());

    // Display the result after LLL reduction
    std::cout << "\nBasis Vectors after LLL reduction:" << std::endl;
    for (const auto& basis_vector : basis_vectors) {
        for (double component : basis_vector.components) {
            std::cout << component << ' ';
        }
        std::cout << std::endl;
    }

    // Solve the Shortest Vector Problem after LLL reduction
    Vector result_vector_after_lll = shortest_vector_bruteforce(basis_vectors);

    // Display the result after LLL reduction
    std::cout << "\nShortest Vector after LLL reduction:" << std::endl;
    for (double component : result_vector_after_lll.components) {
        std::cout << component << ' ';
    }

    double shortest_vector = euclidean_norm(result_vector_after_lll);
    std::cout << "\nEuclidean Norm:\n" << shortest_vector << std::endl;

    // save to result.txt
    std::ofstream file("result.txt");
    file << shortest_vector;
    file.close();

    return 0;
}