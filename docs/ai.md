# Contexto para LLM

O projeto chama-se **Simple Container Manager** e é uma aplicação Desktop desenvolvida em **C++17** utilizando **Qt Widgets** e **CMake**, executada principalmente em Linux Mint.

O objetivo é criar uma ferramenta semelhante ao XAMPP para gerenciamento simplificado de contêineres Docker voltados para ambientes de ensino e desenvolvimento.

Características do projeto:

* Interface construída com Qt Widgets (não utilizar Qt Quick/QML).
* Comunicação com Docker exclusivamente por meio da Docker CLI utilizando `QProcess`.
* Atualização automática do estado dos contêineres através de `docker events`.
* Apenas contêineres previamente configurados devem aparecer na interface.
* Um arquivo de configuração define:

  * nome do contêiner;
  * nome de exibição;
  * ícone opcional;
  * ação padrão.

O desenvolvedor possui pouca experiência com C++ e está utilizando este projeto para aprender:

* conceitos modernos de C++;
* gerenciamento de memória;
* sinais e slots do Qt;
* programação assíncrona e orientada a eventos;
* testes automatizados em Qt;
* boas práticas de arquitetura para aplicações Desktop.

Ao responder perguntas:

* priorize explicações didáticas em vez de fornecer soluções completas;
* explique conceitos específicos de C++ e Qt quando necessário;
* prefira exemplos pequenos e isolados;
* incentive boas práticas de design e arquitetura;
* considere que o projeto utiliza uma abordagem orientada a objetos tradicional com Qt Widgets e não frameworks adicionais.

# Especificações

 * **Memory Hygiene:** Standard stack allocation and value types are heavily preferred over manual heap pointers (new). Qt’s native parent-child layout hierarchy handles widget lifecycles safely.
 * **Const & Optimizations:** Immutable fields are marked const by default. Functions return by non-const value to guarantee compiler move semantics. The mutable keyword is reserved for thread-safe caching. Core functions use std::optional<T> instead of unsafe out-parameter references (bool&).
 * **Configuration & Enums:** Parses its preferences.yaml from a dedicated directory inside QStandardPaths::AppConfigLocation. Enums representing core container states leverage Qt's Meta-Object System (Q\_ENUM) or static-lookup array loops to cleanly process lowercase raw Docker CLI outputs.
 * **Naming Convention:** Private member attributes explicitly use the m\_ prefix (e.g., m\_proc) to clarify scope boundaries and optimize IDE autocomplete functionality.
