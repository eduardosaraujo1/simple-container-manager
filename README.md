### Introdução

[Docker Containers](https://www.docker.com/resources/what-container/) são ideais para a configuração de ambientes de desenvolvimento super flexíveis e estáveis, pois permitem desenvolver em diversas _stacks_ de desenvolvimento no mesmo sistema sem as complicações de instalar linguagens e bibliotecas diretamente na máquina raíz.

Entretanto, a maior parte dos professores e mestres não fazem uso dessa tecnologia, e optam por instalar linguagens e bibliotecas no sistema principal, como o [XAMPP](https://www.apachefriends.org/pt_br/index.html), por exemplo, para ensinar essa ferramenta de desenvolvimento para seus alunos. A justificativa é complexidade: apesar de existirem ferramentas como [Docker Desktop](https://docs.docker.com/desktop/) e [Portainer](https://www.portainer.io/) que facilitam a gestão de Containers, a complexidade dessas ferramentas dificultam seu uso em um ambiente no qual conceitos fundamentais estão sendo ensinados.

Com o objetivo de me familiarizar com o desenvolvimento de aplicações Desktop nativas e a necessidade de uma ferramenta simples para controle de contâiners de desenvolvimento, surge o **Simple Container Manager**: uma ferramenta que abstrai a gestão de contâiners em uma interface parecida com o [XAMPP](https://www.apachefriends.org/pt_br/index.html).

### Tecnologias

Para esse projeto, com o intuito de treinar minha adaptabilidade a novas tecnologias, optei pelas seguintes tecnologias:

- **[Qt-Widgets](https://doc.qt.io/qt-6/topics-ui.html#qt-widgets-user-interfaces)** — Framework de desenvolvimento que abstrai a criação de janelas em ambientes Desktop. Diferente do [Qt-quick](https://doc.qt.io/qt-6/topics-graphics2d.html#qt-quick), a versão utilizada é centrada no desenvolvimento de aplicações Desktop com design simples.
- **[Docker Engine API](https://docs.docker.com/reference/api/engine/)** — API que permite interação com o Docker Engine instalado no sistema. Será utilizado o arquivo `/var/run/docker.sock`, por isso, **a versão inicial terá suporte somente para Linux**.
- **[C++](https://pt.wikipedia.org/wiki/C%2B%2B)** — Linguagem de baixo nível com suporte a orientação a objetos e facilita Separation-of-Concerns.

### Requisitos funcionais

Work in Progress...

### Roadmap

- [ ] Estudar tecnologia Docker Engine API
- [ ] Adaptar-se ao QT-Widgets
- [ ] Estruturar pastas e definir arquitetura modular (MVC? MVVM? UI-Domain-Data?)
