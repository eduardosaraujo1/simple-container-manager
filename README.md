### Introdução

[Docker Containers](https://www.docker.com/resources/what-container/) são ideais para a configuração de ambientes de desenvolvimento super flexíveis e estáveis, pois permitem desenvolver em diversas _stacks_ de desenvolvimento no mesmo sistema sem as complicações de instalar linguagens e bibliotecas diretamente na máquina raíz.

Entretanto, a maior parte dos professores e mestres não fazem uso dessa tecnologia, e optam por instalar linguagens e bibliotecas no sistema principal, como o [XAMPP](https://www.apachefriends.org/pt_br/index.html), por exemplo, para ensinar essa ferramenta de desenvolvimento para seus alunos. A justificativa é complexidade: apesar de existirem ferramentas como [Docker Desktop](https://docs.docker.com/desktop/) e [Portainer](https://www.portainer.io/) que facilitam a gestão de Containers, a complexidade dessas ferramentas dificultam seu uso em um ambiente no qual conceitos fundamentais estão sendo ensinados.

Com o objetivo de me familiarizar com o desenvolvimento de aplicações Desktop nativas e a necessidade de uma ferramenta simples para controle de contâiners de desenvolvimento, surge o **Simple Container Manager**: uma ferramenta que abstrai a gestão de contâiners em uma interface parecida com o [XAMPP](https://www.apachefriends.org/pt_br/index.html).

### Como Utilizar

_Observação: essa versão é hard-coded para utilizar o arquivo localizado em `/var/run/docker.sock`. Por isso, funciona somente em distros Linux com o Docker Engine instalado._

1. Crie os contâiners necessários para desenvolvimento via Docker
2. Abra o aplicativo através da versão disponível na seção Releases ou compile usando cmake e Qt
    - No momento, nenhuma build foi realizada ainda. Por isso nenhum arquivo BUILD.md será disponibilizado.
3. Clique no ícone de "configurações" e ative os contâiners que deseja gerenciar, opcionalmente adicionando um nome (label) à ele.
4. Sua configuração persistirá através de reboots! Recomenda-se utilizar uma ferramenta como Time Freeze para evitar alterações de configuração por parte de usuários pós-configuração.

### Tecnologias

Para esse projeto, com o intuito de treinar minha adaptabilidade a novas tecnologias, optei pelas seguintes tecnologias:

- **[Qt-Widgets](https://doc.qt.io/qt-6/topics-ui.html#qt-widgets-user-interfaces)** — Framework de desenvolvimento que abstrai a criação de janelas em ambientes Desktop. Diferente do [Qt-quick](https://doc.qt.io/qt-6/topics-graphics2d.html#qt-quick), a versão utilizada é centrada no desenvolvimento de aplicações Desktop com design simples.
- **[Docker Engine API](https://docs.docker.com/reference/api/engine/)** — API que permite interação com o Docker Engine instalado no sistema. Será utilizado o arquivo `/var/run/docker.sock`, por isso, **a versão inicial terá suporte somente para Linux**.
- **[C++](https://pt.wikipedia.org/wiki/C%2B%2B)** — Linguagem de baixo nível com suporte a orientação a objetos e facilita Separation-of-Concerns.

### Requisitos Funcionais

**Dicionário:**

- Administrador: usuário que realiza a instalação e configuração;
- Desenvolvedor: usuário que usa simple-container-manager para executar código;
- Aplicação: Docker Container marcado para ser exibido no simple-container-manager.

**RF1 - Seleção de contâiners para exibição**
**Descrição:** O sistema, por se tratar de uma interface de fácil uso, só deve listar alguns Docker Containers, mas não todos. Por isso, através de uma tela de que lista os contâiners criados no sistema, o administrador poderá marcar quais contâiners devem ou não serão tratados como aplicações, assim como especificar uma label (etiqueta) a ser exibida como nome da aplicação. Essa configuração DEVE persistir entre inicializações do aplicativo.
**Prioridade:** Must-Have

**RF2 - Gerenciar aplicações na tela inicial**
**Descrição:** Ao abrir o aplicativo, uma lista de todas as aplicações previamente configuradas devem estar disponíveis em forma de lista para serem ativadas (container on), desativadas (container off) e executadas (executar comando especificado na configuração).
**Prioridade:** Must-Have

**RF3 - Arquivo de trava**
**Descrição:** Após a configuração ser concluida, um arquivo de trava poderá ser criado que, ao perceber sua existência, o aplicativo esconderá o menu de configuração da aplicação. Note que isso **não é uma forma segura de previnir adulteração das configurações**, pois configs ainda podem ser modificadas forçadamente por meio do arquivo de configuração. Embora em tese seja possível reduzir essa possibilidade através de Linux File System Permissions e mitigar acesso `root`, a melhor coisa a se fazer é criar um mecanismo de redefinição da máquina após o uso (time freeze).
**Prioridade:** Could-Have

### Casos de Uso

1. verAplicacoes
2. ativarAplicacao
3. desativarAplicacao
4. abrirAplicacao
5. refreshAplicacoes
6. listarContainers
7. refreshContainers
8. marcarContainerAplicacao
9. desmarcarContainerAplicacao
10. alterarLabelContainer
11. alterarRunCommandContainer

### Roadmap

- [x] Definir escopo
- [ ] Estudar tecnologia Docker Engine API com C++
- [ ] Estruturar pastas e definir arquitetura modular (MVC? MVVM? UI-Domain-Data? Qual é melhor para o uso de Qt Widgets?)
- [ ] Verificar possibilidade de testes de integração para essa stack
- [ ] Criar camada de abstração para comunicação com docker socket
- [ ] Adaptar-se ao Qt Widgets e aprender melhores práticas
- [ ] Desenhar UI com Qt Widgets, consumindo a abstração docker
