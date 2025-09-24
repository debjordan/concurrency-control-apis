#include <gtest/gtest.h>
#include <atomic>
#include "../include/thread_pool/thread_pool.h"

/**
 * @brief Testes unitários para ThreadPool
 *
 * Verifica funcionalidade básica, tratamento de exceções e
 * comportamento em condições de borda.
 */
class ThreadPoolTest : public ::testing::Test {
protected:
    void SetUp() override {
        pool = std::make_unique<ThreadPool>(4);
    }

    void TearDown() override {
        pool.reset();
    }

    std::unique_ptr<ThreadPool> pool;
};

/**
 * @brief Testa execução básica de tarefas
 */
TEST_F(ThreadPoolTest, ExecucaoBasica) {
    std::atomic<int> counter{0};
    const int NUM_TASKS = 100;

    std::vector<std::future<void>> futures;
    for (int i = 0; i < NUM_TASKS; ++i) {
        futures.push_back(pool->submit([&counter]() {
            counter++;
        }));
    }

    // Espera todas as tarefas completarem
    for (auto& future : futures) {
        future.wait();
    }

    EXPECT_EQ(counter.load(), NUM_TASKS);
}

/**
 * @brief Testa retorno de valores das tarefas
 */
TEST_F(ThreadPoolTest, RetornoValores) {
    auto task = [](int a, int b) { return a + b; };

    auto future = pool->submit(task, 10, 20);
    int result = future.get();

    EXPECT_EQ(result, 30);
}

/**
 * @brief Testa tratamento de exceções
 */
TEST_F(ThreadPoolTest, TratamentoExcecoes) {
    auto throwing_task = []() {
        throw std::runtime_error("Erro simulado");
        return 42;
    };

    auto future = pool->submit(throwing_task);

    EXPECT_THROW(future.get(), std::runtime_error);
}

/**
 * @brief Testa comportamento com pool vazio
 */
TEST(ThreadPoolSingleThreadTest, PoolSingleThread) {
    ThreadPool single_thread_pool(1);
    std::atomic<int> counter{0};

    // Submete tarefas que devem executar sequencialmente
    std::vector<std::future<void>> futures;
    for (int i = 0; i < 10; ++i) {
        futures.push_back(single_thread_pool.submit([&counter, i]() {
            counter += i;
        }));
    }

    for (auto& future : futures) {
        future.wait();
    }

    // Soma de 0 a 9 = 45
    EXPECT_EQ(counter.load(), 45);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
