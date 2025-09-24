#include <iostream>
#include <thread>
#include <vector>
#include "../include/resource_manager/resource_manager.h"

/**
 * @brief Exemplo de uso do ResourceManager
 *
 * Demonstra controle de acesso concorrente a recursos compartilhados
 * com locks de leitura e escrita.
 */
int main() {
    std::cout << "=== Exemplo Resource Manager ===" << std::endl;

    // Cria gerenciador para recursos do tipo int
    ResourceManager<std::string, int> resource_manager;

    // Adiciona alguns recursos
    resource_manager.add_resource("contador", std::make_shared<int>(0));
    resource_manager.add_resource("total", std::make_shared<int>(100));

    std::cout << "Recursos adicionados: contador=0, total=100" << std::endl;

    // Função para leitores (múltiplos podem acessar simultaneamente)
    auto reader_function = [&resource_manager](int id) {
        for (int i = 0; i < 3; ++i) {
            // Obtém acesso de leitura ao contador
            auto read_lock = resource_manager.get_read_access("contador");
            int valor = *read_lock;

            std::cout << "Leitor " << id << " leu contador: " << valor << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    };

    // Função para escritores (acesso exclusivo)
    auto writer_function = [&resource_manager](int id) {
        for (int i = 0; i < 2; ++i) {
            // Obtém acesso de escrita ao contador
            auto write_lock = resource_manager.get_write_access("contador");
            (*write_lock)++;  // Incrementa o contador

            std::cout << "Escritor " << id << " incrementou contador para: "
                     << *write_lock << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
    };

    // Cria threads leitoras e escritoras
    std::vector<std::thread> threads;

    // 3 leitores
    for (int i = 0; i < 3; ++i) {
        threads.emplace_back(reader_function, i);
    }

    // 2 escritores
    for (int i = 0; i < 2; ++i) {
        threads.emplace_back(writer_function, i);
    }

    // Aguarda todas as threads
    for (auto& t : threads) {
        t.join();
    }

    // Resultado final
    auto final_lock = resource_manager.get_read_access("contador");
    std::cout << "\nValor final do contador: " << *final_lock << std::endl;

    std::cout << "Exemplo concluído com sucesso!" << std::endl;
    return 0;
}
