I've created a thread-safe thread pool implementation in C!

it will use thread-safe queu using pthread mutex,
the purpose is to implement a complete pool impelemtnation with workers thread inspired by the project of `dining philosopher`

We use `void *` arguments for managing all type context

Needed to manage explictlyt the dynamic allocations malloc/free instead of RAII

As it's a pool I thought that iwas more conveneint to transform teh queue structure as a linked list