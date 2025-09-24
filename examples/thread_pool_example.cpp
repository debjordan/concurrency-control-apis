#include <iostream>
#include <vector>
#include <chrono>
#include "../include/thread_pool/thread_pool.h"

/**
 * @brief Exemplo de uso do ThreadPool
 *
 * Demonstra submissão de tarefas, recuperação de resultados e
 * controle de concorrência.
 */
int main() {
    std::cout << "=== Exemplo Thread Pool ===" << std::endl;

    // Cria pool com 4 threads
    ThreadPool pool(4);
    std::cout << "ThreadPool criado com " << pool.size() << " threads" << std::endl;

    // Vetor para armazenar os futures dos resultados
    std::vector<std::future<int>> results;

    // Submete 10 tarefas para execução
    for (int i = 0; i < 10; ++i) {
        // Cada tarefa retorna seu índice após simular trabalho
        results.emplace_back(
            pool.submit([i]() {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                std::cout << "Tarefa " << i << " executada na thread "
                         << std::this_thread::get_id() << std::endl;
                return i * i;
            })
        );
    }

    // Coleta os resultados
    std::cout << "\nColetando resultados:" << std::endl;
    for (size_t i = 0; i < results.size(); ++i) {
        int result = results[i].get();  // Bloqueia até resultado disponível
        std::cout << "Resultado da tarefa " << i << ": " << result << std::endl;
    }

    std::cout << "Exemplo concluído com sucesso!" << std::endl;
    return 0;
}
