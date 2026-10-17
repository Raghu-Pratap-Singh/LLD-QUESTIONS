/*
    condition variables:
    lets consider producer consumer problem

    producer->pushes message to message queue
    consumer->consumes message from message queue

    message_queue->shared resource between consumer thread and producer thread

    now we know that consumer loop goes like
    while (1) {
        lock_guard<> lock()
        ...
    }

    PROBLEM:
    now the consumer loop runs forever means if the producer wnats to insert a new message, it will need that lock to be freed=>but since loop in continous , it wont happen=>in such cases condition variables comes in play

    SOLUTION: 
    the problem is happening because the consumer thread is continously on lookout for messages in message queue, if somehow consumer goes on sleep and when the producer inserts a message in message queue, consumer can be notified, then it can be solved, this can be done using a shared variable(condition variable)
*/