## Exercise 3 - The sentences below are taken from the Architecture Reference Manual for ARM64 processors

### a) Privilege Levels in ARM64 Processors

#### Problem:
«The SVC instruction causes a Supervisor Call exception. This provides a mechanism for unprivileged software to make a system call (...).» (B1.3.1)

«A Supervisor Call enables software executing at EL0 to make a call to (...) software executing at EL1.» (D1.3.9)

**a.** Knowing that EL0 and EL1 correspond to privilege levels in ARM64 processors, based on the text, indicate which of the values is the most privileged and which is the least privileged.

#### Answer:
A palavra-chave "permite" (enables) no texto sugere que o software em **EL0** precisa de autorização para fazer chamadas para o software em **EL1**. Portanto:

- **EL1** é o **mais privilegiado** porque o software em EL0 depende dele para fazer chamadas.
- **EL0** é o **menos privilegiado** porque requer permissões para realizar chamadas ao nível superior, EL1.


---

### b) SVC Instruction and System Call Mechanism

#### Problem:
The SVC instruction is described as an exception and it does not have as an argument the code address where to transfer execution. What is the relevance of this and also, in the specific case of Linux, how does the system know which of the various possible system calls is to be executed?

#### Answer:
A instrução SVC (Supervisor Call) é descrita como uma exceção porque permite que o código em nível menos privilegiado solicite serviços de software mais privilegiado (EL1). A instrução não requer o endereço do código para onde a execução deve ser transferida porque a interface de chamadas de sistema tem um ponto de entrada fixo no kernel, que é conhecido antecipadamente.

No Linux, quando a instrução SVC é executada, o sistema determina qual chamada de sistema deve ser executada com base no valor colocado num registo predefinido (normalmente `x8` no ARM64). O número neste registo corresponde ao número da chamada de sistema, e o kernel usa esse valor para procurar e invocar a chamada de sistema correta.

-----

## Exercise 4 - The following source code of a library (file with .so extension or shared object) is intended to be loaded using the `dlopen` function

```c
int count = 1;

void *xalloc(size_t npages) {
    ++count;
    return mmap(NULL, npages * 4096, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
}

void xfree(void *ptr) {
    --count;
    munmap(ptr);
}
```



### a) Process Address Space Change


A process loaded the library and only later invoked `xalloc(4)`. What changed in the process's address space (which is visible in `/proc/<pid>/smaps`) with this invocation?

#### Answer:
A invocação da função `xalloc(4)` aloca 4 páginas de memória (4 * 4096 bytes = 16 KiB) no espaço de endereçamento do processo. Essa alocação é feita através da função `mmap`, o que significa que novas regiões de memória privadas são criadas no processo. No ficheiro `/proc/<pid>/smaps`, será visível uma nova região de memória de 16 KiB, com permissões de leitura e escrita (`rw-`), mas sem permissão de execução.

---

### b) Address Space Change on `dlclose`


If the process invokes `dlclose` when the value of `count` is 2, what will change in the address space (visible in `/proc/<pid>/smaps`)?

#### Answer:
Quando o processo invoca `dlclose` com o valor de `count = 2`, a função `xfree` será chamada, o que resulta na execução de `munmap` para libertar a memória apontada. Como o `count` diminui de 2 para 1, a região de memória de 4 páginas alocada anteriormente será removida do espaço de endereçamento do processo. No ficheiro `/proc/<pid>/smaps`, essa região de memória não será mais visível, indicando que a memória foi libertada com sucesso.
