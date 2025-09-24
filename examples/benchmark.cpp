#include <iostream>
#include <chrono>
#include <atomic>
#include <vector>
#include "../include/thread_pool/thread_pool.h"

/**
 * @brief Benchmark para testar performance do ThreadPool
 *
 * Compara execução sequencial vs concorrente e mede speedup.
 */
int main() {
    std::cout << "=== Benchmark Thread Pool ===" << std::endl;

    const int NUM_TAREFAS = 1000;
    const int WORK_PER_TASK = 10000;  // Trabalho simulado por tarefa

    // Função que simula trabalho (cálculo intensivo)
    auto heavy_work = [](int n) {
        long result = 0;
        for (int i = 0; i < WORK_PER_TASK; ++i) {
            result += i * n;
        }
        return result;
    };

    // Benchmark execução sequencial
    std::cout << "Executando benchmark sequencial..." << std::endl;
    auto start_seq = std::chrono::high_resolution_clock::now();

    std::vector<long> results_seq(NUM_TAREFAS);
    for (int i = 0; i < NUM_TAREFAS; ++i) {
        results_seq[i] = heavy_work(i);
    }

    auto end_seq = std::chrono::high_resolution_clock::now();
    auto duration_seq = std::chrono::duration_cast<std::chrono::milliseconds>(end_seq - start_seq);

    // Benchmark execução com ThreadPool
    std::cout << "Executando benchmark concorrente..." << std::endl;
    auto start_conc = std::chrono::high_resolution_clock::now();

    ThreadPool pool(std::thread::hardware_concurrency());
    std::vector<std::future<long>> futures;

    for (int i = 0; i < NUM_TAREFAS; ++i) {
        futures.push_back(pool.submit(heavy_work, i));
    }

    std::vector<long> results_conc(NUM_TAREFAS);
    for (int i = 0; i < NUM_TAREFAS; ++i) {
        results_conc[i] = futures[i].get();
    }

    auto end_conc = std::chrono::high_resolution_clock::now();
    auto duration_conc = std::chrono::duration_cast<std::chrono::milliseconds>(end_conc - start_conc);

    // Verifica que os resultados são iguais
    bool results_match = (results_seq == results_conc);

    // Calcula speedup
    double speedup = static_cast<double>(duration_seq.count()) / duration_conc.count();

    // Apresenta resultados
    std::cout << "\n=== Resultados do Benchmark ===" << std::endl;
    std::cout << "Número de tarefas: " << NUM_TAREFAS << std::endl;
    std::cout << "Threads no pool: " << pool.size() << std::endl;
    std::cout << "Tempo sequencial: " << duration_seq.count() << "ms" << std::endl;
    std::cout << "Tempo concorrente: " << duration_conc.count() << "ms" << std::endl;
    std::cout << "Speedup: " << speedup << "x" << std::endl;
    std::cout << "Resultados consistentes: " << (results_match ? "SIM" : "NÃO") << std::endl;

    if (speedup > 1.0) {
        std::cout << "✓ Concorrência melhorou a performance!" << std::endl;
    } else {
        std::cout << "⚠ Overhead da concorrência impactou performance" << std::endl;
    }

    return 0;
}
