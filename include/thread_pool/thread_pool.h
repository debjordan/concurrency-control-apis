#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>
#include "task_queue.h"
#include "worker_thread.h"

/**
 * @class ThreadPool
 * @brief Pool de threads para execução concorrente de tarefas
 *
 * Esta classe gerencia um conjunto de threads workers que processam tarefas
 * de forma assíncrona. Oferece interface para submeter tarefas e recuperar
 * seus resultados via futures.
 */
class ThreadPool {
public:
    /**
     * @brief Construtor que inicializa o pool com número específico de threads
     * @param num_threads Número de threads no pool (padrão: número de cores hardware)
     */
    explicit ThreadPool(size_t num_threads = std::thread::hardware_concurrency());

    /**
     * @brief Destrutor que para todas as threads e espera conclusão
     */
    ~ThreadPool();

    /**
     * @brief Submete uma tarefa para execução no pool
     * @tparam F Tipo da função a ser executada
     * @tparam Args Tipos dos argumentos da função
     * @param f Função a ser executada
     * @param args Argumentos para a função
     * @return Future com o resultado da execução
     */
    template<class F, class... Args>
    auto submit(F&& f, Args&&... args)
        -> std::future<typename std::result_of<F(Args...)>::type>;

    /**
     * @brief Retorna o número de threads ativas no pool
     * @return Número de threads
     */
    size_t size() const;

    /**
     * @brief Verifica se o pool está parado
     * @return true se parado, false caso contrário
     */
    bool stopped() const;

private:
    std::vector<std::unique_ptr<WorkerThread>> workers; ///< Vetor de threads workers
    std::shared_ptr<TaskQueue> task_queue;              ///< Fila compartilhada de tarefas
    bool stop;                                          ///< Flag de parada
};

// Implementação do template (deve estar no header)
template<class F, class... Args>
auto ThreadPool::submit(F&& f, Args&&... args)
    -> std::future<typename std::result_of<F(Args...)>::type> {

    using return_type = typename std::result_of<F(Args...)>::type;

    // Cria um packaged_task com a função e argumentos
    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
    );

    std::future<return_type> result = task->get_future();

    // Adiciona tarefa à fila
    if (!task_queue->push([task](){ (*task)(); })) {
        throw std::runtime_error("ThreadPool está parado, não pode aceitar tarefas");
    }

    return result;
}

#endif
