# Simple Container Manager

Uma interface gráfica simples para gerenciamento de contêineres Docker voltada para ambientes educacionais e de desenvolvimento.

## Sobre o Projeto

O Docker permite criar ambientes de desenvolvimento flexíveis, reproduzíveis e isolados, evitando a instalação de linguagens, bibliotecas e serviços diretamente no sistema operacional.

Apesar dessas vantagens, ferramentas de gerenciamento como Docker Desktop e Portainer podem ser excessivamente complexas para ambientes de ensino, especialmente quando o objetivo é introduzir conceitos básicos de programação e desenvolvimento web.

O **Simple Container Manager** nasceu com dois objetivos:

- fornecer uma experiência semelhante ao XAMPP para gerenciamento de serviços;
- servir como projeto de aprendizado em desenvolvimento de aplicações Desktop nativas utilizando C++ e Qt.

## Como Funciona

O Simple Container Manager não exibe todos os contêineres existentes no Docker Engine.

Um arquivo de configuração define quais contêineres serão apresentados na interface, além de suas propriedades visuais:

- nome interno do contêiner;
- nome de exibição;
- ícone opcional;
- ação padrão.

Isso permite que um administrador prepare um ambiente de desenvolvimento para estudantes sem exigir conhecimento prévio de Docker.

## Funcionalidades

- Exibição apenas de contêineres previamente configurados;
- Inicialização e parada de aplicações com um clique;
- Atualização automática de status utilizando eventos do Docker;
- Configuração de nome amigável e ícone para cada aplicação;
- Atualização manual da lista de aplicações e das configurações;
- Visualização de logs em tempo real.

## Capturas de Tela

> Em desenvolvimento.

## Instalação

### Utilizando uma Release

1. Baixe a versão mais recente na seção **Releases**.
2. Execute o aplicativo.

> Ainda não existem builds oficiais disponíveis.

### Compilando o Projeto

Pré-requisitos:

- CMake;
- Qt 6 (Widgets);
- Docker CLI;
- Compilador compatível com C++20 ou superior.

```bash
git clone <repositorio>
cd simple-container-manager
mkdir build
cd build
cmake ..
cmake --build .
```

## Uso

1. Crie os contêineres necessários utilizando Docker.
2. Abra o Simple Container Manager.
3. Acesse a tela de configurações.
4. Selecione os contêineres que deverão ser gerenciados.
5. Opcionalmente, personalize nomes e ícones.

## Documentação Técnica

<details>

<summary>Clique para expandir</summary>

### Tecnologias Utilizadas

- **Qt Widgets** — Interface gráfica Desktop nativa.
- **Docker CLI** — Comunicação com o Docker Engine por meio da linha de comando.
- **C++** — Linguagem utilizada para implementação da aplicação.

### Requisitos Funcionais

#### RF1 – Seleção de Contêineres

O administrador pode definir quais contêineres serão exibidos pela aplicação, incluindo:

- nome;
- etiqueta (label);
- ícone;
- ação padrão.

**Prioridade:** Must Have.

#### RF2 – Gerenciamento de Aplicações

A tela principal deve exibir:

- etiqueta;
- ícone;
- status;
- botões de iniciar, parar e reiniciar.

**Prioridade:** Must Have.

#### RF3 – Atualização de Status

A aplicação deve manter sincronização com o Docker Engine utilizando eventos do Docker e oferecer atualização manual caso necessário.

**Prioridade:** Should Have.

#### RF4 – Exibição de Logs

Os logs de uma aplicação poderão ser exibidos em uma janela dedicada em tempo real.

### Casos de Uso

1. Listar aplicações;
2. Ativar aplicação;
3. Desativar aplicação;
4. Recarregar aplicações;

# Roadmap

- [x] Definir escopo
- [x] Criar camada de abstração para comunicação com docker (ativar e desativar container utilizando nome; equivalente de Stream para reagir a alterações de uma thread `docker events --format json`)
- [ ] Criar UI para exibir e controlar os containers

</details>

## Licença

Ainda não definida.
