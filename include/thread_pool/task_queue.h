#ifndef TASK_QUEUE_H
#define TASK_QUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>

/**
 * @class TaskQueue
 * @brief Fila thread-safe para armazenamento de tarefas
 *
 * Implementa uma fila bloqueante que permite produção e consumo seguro
 * de tarefas entre múltiplas threads.
 */
class TaskQueue {
public:
    using Task = std::function<void()>; ///< Tipo da tarefa (função sem retorno)

    /**
     * @brief Construtor padrão
     */
    TaskQueue();

    /**
     * @brief Adiciona uma tarefa à fila
     * @param task Tarefa a ser adicionada
     * @return true se bem-sucedido, false se a fila está parada
     */
    bool push(Task task);

    /**
     * @brief Remove e retorna uma tarefa da fila (bloqueante)
     * @param task Referência para armazenar a tarefa removida
     * @return true se obteve tarefa, false se a fila está parada e vazia
     */
    bool pop(Task& task);

    /**
     * @brief Para a fila, acordando todas as threads bloqueadas
     */
    void stop();

    /**
     * @brief Verifica se a fila está parada
     * @return true se parada, false caso contrário
     */
    bool stopped() const;

    /**
     * @brief Retorna o tamanho atual da fila
     * @return Número de tarefas na fila
     */
    size_t size() const;

private:
    mutable std::mutex mutex;                   ///< Mutex para sincronização
    std::condition_variable condition;          ///< Variável de condição para espera
    std::queue<Task> queue;                     ///< Fila interna de tarefas
    bool stop_flag;                             ///< Flag de parada
};

#endif
