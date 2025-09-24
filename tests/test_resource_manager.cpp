#include <gtest/gtest.h>
#include <thread>
#include <vector>
#include <atomic>
#include "../include/resource_manager/resource_manager.h"

/**
 * @brief Testes unitários para ResourceManager
 *
 * Verifica controle de acesso, consistência de dados e
 * prevenção de race conditions.
 */
class ResourceManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        manager.add_resource("data", std::make_shared<int>(0));
        manager.add_resource("config", std::make_shared<int>(100));
    }

    ResourceManager<std::string, int> manager;
};

/**
 * @brief Testa acesso básico de leitura e escrita
 */
TEST_F(ResourceManagerTest, AcessoBasico) {
    {
        auto write_lock = manager.get_write_access("data");
        *write_lock = 42;
    }

    {
        auto read_lock = manager.get_read_access("data");
        EXPECT_EQ(*read_lock, 42);
    }
}

/**
 * @brief Testa múltiplos leitores simultâneos
 */
TEST_F(ResourceManagerTest, MultiplosLeitores) {
    std::atomic<int> readers_active{0};
    std::atomic<bool> writer_started{false};
    const int NUM_READERS = 5;

    std::vector<std::thread> readers;
    std::thread writer;

    // Cria leitores
    for (int i = 0; i < NUM_READERS; ++i) {
        readers.emplace_back([&, i]() {
            auto read_lock = manager.get_read_access("data");
            readers_active++;

            // Espera writer tentar iniciar
            while (!writer_started) {
                std::this_thread::yield();
            }

            // Leitores devem conseguir acessar simultaneamente
            EXPECT_GT(readers_active.load(), 0);
            readers_active--;
        });
    }

    // Cria escritor (deve esperar leitores terminarem)
    writer = std::thread([&]() {
        writer_started = true;
        auto write_lock = manager.get_write_access("data");

        // Writer só deve conseguir acesso quando nenhum leitor estiver ativo
        EXPECT_EQ(readers_active.load(), 0);
        *write_lock = 100;
    });

    // Aguarda todas as threads
    for (auto& reader : readers) {
        reader.join();
    }
    writer.join();

    // Verifica valor final
    auto read_lock = manager.get_read_access("data");
    EXPECT_EQ(*read_lock, 100);
}

/**
 * @brief Testa acesso a recurso inexistente
 */
TEST_F(ResourceManagerTest, RecursoInexistente) {
    EXPECT_THROW(manager.get_read_access("inexistente"), std::runtime_error);
    EXPECT_THROW(manager.get_write_access("inexistente"), std::runtime_error);
}

/**
 * @brief Testa adição e remoção de recursos
 */
TEST_F(ResourceManagerTest, GerenciamentoRecursos) {
    EXPECT_TRUE(manager.contains("data"));
    EXPECT_TRUE(manager.contains("config"));
    EXPECT_FALSE(manager.contains("novo"));

    manager.add_resource("novo", std::make_shared<int>(200));
    EXPECT_TRUE(manager.contains("novo"));

    manager.remove_resource("data");
    EXPECT_FALSE(manager.contains("data"));
    EXPECT_THROW(manager.get_read_access("data"), std::runtime_error);
}

/**
 * @brief Testa consistência sob concorrência intensa
 */
TEST_F(ResourceManagerTest, ConcorrenciaIntensa) {
    const int NUM_OPERATIONS = 1000;
    std::atomic<int> successful_writes{0};
    std::vector<std::thread> threads;

    for (int i = 0; i < 4; ++i) {
        threads.emplace_back([&, i]() {
            for (int j = 0; j < NUM_OPERATIONS; ++j) {
                if (j % 10 == 0) {  // A cada 10 operações, escreve
                    auto write_lock = manager.get_write_access("data");
                    (*write_lock)++;
                    successful_writes++;
                } else {  // Lê nas outras operações
                    auto read_lock = manager.get_read_access("data");
                    int value = *read_lock;
                    EXPECT_GE(value, 0);
                }
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    // Verifica que todas as escritas foram bem-sucedidas
    auto final_lock = manager.get_read_access("data");
    EXPECT_EQ(*final_lock, successful_writes.load());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
