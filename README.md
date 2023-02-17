In this homework, we were asked to implement a deadlock-free solution dining philosophers problem. The problem can be solved either by using semaphores or monitors. In my work, I tried to implement a monitor solution. The distribution of chopsticks is controlled via a monitor.
In the code, each philosopher is initialized in thinking state. Then when they get hungry, they need to check whether neither of their neighbors are eating. If they are not using their chopsticks, philosopher(i) can change his state to eating. Otherwise, he will stay in the hungry state.
You can run the code with the aid of a make command
- Open the directory in terminal
- First, run the command make
- Then, run the command /.phsp