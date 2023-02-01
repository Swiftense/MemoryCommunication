<h1>MemoryCommunication</h1>
<p>Swiftense communicates due Memory Packets to Clients on the local machine.</p>
<h2>Why dont't just use Network Packets?</h2>
Packets sent through shared memory can be over 15x faster than Packets sent through a network socket using TCP. But furthermore a lot of times The Database server and client are on the same machine and then it makes no sense to not only waste a lot of CPU power but also loose the security advantages of network packets.
<h2>How does it work?</h2>
<p>The Memory Server starts by opening up a Shared Memory buffer using the <code>shmget</code> and <code>shmat</code> systemcalls, and opening up an semaphore usign the <code>sem_open</code> systemcall. And then for sending and recieving messages a hybrid algorithm between a semaphore and a spinlock is used for the optimal balance between latency and performance.</p>
<h2>Example</h2>
<p>Note that this doesn't work on systems without bash, and that the <strong>GNU Compiler Collection</strong> has to be installed. Start by cloning this repository.</p>

```bash
git clone https://github.com/Swiftense/MemoryCommunication
cd MemoryCommunication
chmod +x client.sh && server.sh
```

<p>Then you can run <code>./server.sh messanger</code> and <code>./client.sh messanger</code> to open a simple prompt that allows you to sent messages between client and server. But you can also run the scripts with the <strong>performance</strong> argument if you want to benchmark the performance or the <strong>count</strong> argument to just make the client and server counting.</p>
<h2>Problems</h2>
<ul>
    <li>Everything in this repository is under heavy development and has no gauranty to work.</li>
    <li>No specific protocol is used.</li>
    <li>Currently no permission Management.</li>
</ul>
