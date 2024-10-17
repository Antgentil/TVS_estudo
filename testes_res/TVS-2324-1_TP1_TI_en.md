
![Exercise 3](https://github.com/Antgentil/TVS_estudo/raw/main/testes_imgs/TVS-2324-1_TP1_TI_en-3.png)


### a) Privilege Levels in ARM64 Processors

A palavra-chave "permite" (enables) no texto sugere que o software em **EL0** precisa de autorização para fazer chamadas para o software em **EL1**. Portanto:

- **EL1** é o **mais privilegiado** porque o software em EL0 depende dele para fazer chamadas.
- **EL0** é o **menos privilegiado** porque requer permissões para realizar chamadas ao nível superior, EL1.

---

### b) SVC Instruction and System Call Mechanism

A instrução SVC (Supervisor Call) é descrita como uma exceção porque permite que o código em nível menos privilegiado solicite serviços de software mais privilegiado (EL1). A instrução não requer o endereço do código para onde a execução deve ser transferida porque a interface de chamadas de sistema tem um ponto de entrada fixo no kernel, que é conhecido antecipadamente.

No Linux, quando a instrução SVC é executada, o sistema determina qual chamada de sistema deve ser executada com base no valor colocado num registo predefinido (normalmente `x8` no ARM64). O número neste registo corresponde ao número da chamada de sistema, e o kernel usa esse valor para procurar e invocar a chamada de sistema correta.

-----

![Exercise 4](https://github.com/Antgentil/TVS_estudo/raw/main/testes_imgs/TVS-2324-1_TP1_TI_en-4.png)


### a) Process Address Space Change

A invocação da função `xalloc(4)` aloca 4 páginas de memória (4 * 4096 bytes = 16 KiB) no espaço de endereçamento do processo. Essa alocação é feita através da função `mmap`, o que significa que novas regiões de memória privadas são criadas no processo. No ficheiro `/proc/<pid>/smaps`, será visível uma nova região de memória de 16 KiB, com permissões de leitura e escrita (`rw-`), mas sem permissão de execução.

---

### b) Address Space Change on `dlclose`

Quando o processo invoca `dlclose` com o valor de `count = 2`, a função `xfree` será chamada, o que resulta na execução de `munmap` para libertar a memória apontada. Como o `count` diminui de 2 para 1, a região de memória de 4 páginas alocada anteriormente será removida do espaço de endereçamento do processo. No ficheiro `/proc/<pid>/smaps`, essa região de memória não será mais visível, indicando que a memória foi libertada com sucesso.
