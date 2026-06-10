# Introdução

[Docker Containers](https://www.docker.com/resources/what-container/) são ideais para a configuração de ambientes de desenvolvimento super flexíveis e estáveis, pois permitem desenvolver em diversas _stacks_ de desenvolvimento no mesmo sistema sem as complicações de instalar linguagens e bibliotecas diretamente na máquina raíz.

Entretanto, a maior parte dos professores e mestres não fazem uso dessa tecnologia, e optam por instalar linguagens e bibliotecas no sistema principal, como o [XAMPP](https://www.apachefriends.org/pt_br/index.html), por exemplo, para ensinar essa ferramenta de desenvolvimento para seus alunos. A justificativa é complexidade: apesar de existirem ferramentas como [Docker Desktop](https://docs.docker.com/desktop/) e [Portainer](https://www.portainer.io/) que facilitam a gestão de Containers, a complexidade dessas ferramentas dificultam seu uso em um ambiente no qual conceitos fundamentais estão sendo ensinados.

Com o objetivo de me familiarizar com o desenvolvimento de aplicações Desktop nativas e a necessidade de uma ferramenta simples para controle de contâiners de desenvolvimento, surge o **Simple Container Manager**: uma ferramenta que abstrai a gestão de contâiners em uma interface parecida com o [XAMPP](https://www.apachefriends.org/pt_br/index.html).

# Work in Progress: resumo do projeto que pode ser integrado à descrição

The user is creating a Qt Widgets project using Qt Creator on a Linux Mint environment. He does not have much experience on C++, and this is his first project using Qt.

The project is a tool inspired by the ease of use of XAMPP and the flexibility of container technology. A simple UI very similar to XAMPP which allows to list running containers that can be started or stopped, just like XAMPP services.

To prevent unrelated containers from appearing in the UI, a containers-list.config.yaml file will exist to filter which containers should appear, what their display names should be, and (optionally) a display icon for those applications.

The target user-base are system admins who want to use Docker as a base for the development environment of students, who won't have to know docker specifics in order to learn various technologies.

Under the hood, the docker CLI is used with QProcess. `docker events` is used to react dynamically to container status updates. Further decisions are yet to be made.

# Como Utilizar

1. Crie os contâiners necessários para desenvolvimento via Docker
2. Abra o aplicativo através da versão disponível na seção Releases ou compile usando cmake e Qt
    - No momento, nenhuma build foi realizada ainda. Por isso nenhum arquivo BUILD.md será disponibilizado.
3. Clique no botão "configurações" e ative os contâiners que deseja gerenciar, opcionalmente adicionando um nome customizado.

# Tecnologias

Para esse projeto, com o intuito de treinar minha adaptabilidade a novas tecnologias, optei pelas seguintes tecnologias:

- **[Qt-Widgets](https://doc.qt.io/qt-6/topics-ui.html#qt-widgets-user-interfaces)** — Framework de desenvolvimento que abstrai a criação de janelas em ambientes Desktop. Diferente do [Qt-quick](https://doc.qt.io/qt-6/topics-graphics2d.html#qt-quick), a versão utilizada é centrada no desenvolvimento de aplicações Desktop com design simples.
- **[Docker CLI](https://docs.docker.com/reference/cli/docker)** — Será utilizada a interface de linha de comando do Docker (CLI) para gerenciar e interagir com contêineres, imagens e demais recursos.
- **[C++](https://pt.wikipedia.org/wiki/C%2B%2B)** — Linguagem de baixo nível com suporte a orientação a objetos.

# Requisitos Funcionais

## Dicionário

- Administrador: usuário que realiza a instalação e configuração;
- Desenvolvedor: usuário que usa simple-container-manager para executar código;
- Aplicação: Docker Container marcado para ser exibido no simple-container-manager.

## RF1 - Seleção de contâiners para exibição

**Descrição:** Por se tratar de uma interface de fácil uso, o sistema só deve listar alguns Docker Containers, mas não todos. Por isso, através do arquivo de configuração, o administrador poderá especificar o nome do contâiner, um ícone para exibição e uma label (etiqueta).

**Prioridade:** Must-Have

## RF2 - Gerenciar aplicações na tela inicial

**Descrição:** Ao abrir o aplicativo, uma lista de todas as aplicações previamente configuradas devem estar disponíveis em forma de lista para serem ativadas, desativadas e gerenciadas.

**Prioridade:** Must-Have

### Casos de Uso

1. listarAplicacoes
2. ativarAplicacao
3. desativarAplicacao
4. gerenciarAplicacao
5. recarregarConfiguracao

### Roadmap

- [x] Definir escopo
- [ ] Criar camada de abstração para comunicação com docker (ativar e desativar container utilizando nome; equivalente de Stream para reagir a alterações de uma thread `docker events --format json`)
- [ ] Criar Qt Model para representar o estado atual de cada container
- [ ] Criar UI para exibir e controlar os containers
