// K2_pavyzdine.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <mpi.h>

using namespace std;

const int MAX_MESSAGES = 20;

void sendData(int start)
{
    auto rank = MPI::COMM_WORLD.Get_rank();
    cout << "SendData " << rank << ": Started\n";
    int i = start;
    while(true)
    {
        cout << "SendData " << rank << ": Sending: " << i << endl;
        int num = i;
        MPI::COMM_WORLD.Send(&num, 1, MPI::INT, 0, 0);

        bool isFinished = false;
        MPI::COMM_WORLD.Recv(&isFinished, 1, MPI::BOOL, 0, 0);
        cout << "SendData " << rank << ": Response received\n";

        if (isFinished)
            return;

        i++;
    }
}


void processData()
{
    int totalProcessed = 0;

    for (int i = 0; i < MAX_MESSAGES; i++)
    {
        MPI::Status status;
        MPI::COMM_WORLD.Probe(MPI::ANY_SOURCE, MPI::ANY_TAG, status);
        int source = status.Get_source();

        cout << "processData: Waiting for data\n";
        int num;
        MPI::COMM_WORLD.Recv(&num, 1, MPI::INT, source, 0);
        cout << "processData: Sending " << num << " to: " << (num % 2 == 0 ? 3 : 4) << endl;
        MPI::COMM_WORLD.Send(&num, 1, MPI::INT, num % 2 == 0 ? 3 : 4, 0);
        cout << "processData: Sending response to: " << source << endl;
        bool isEnd = false;
        MPI::COMM_WORLD.Send(&isEnd, 1, MPI::BOOL, source, 0);
    }

    cout << "processData: Sending end signals\n";
    bool end = true;
    int num;
    MPI::COMM_WORLD.Recv(&num, 1, MPI::INT, 1, 0);
    MPI::COMM_WORLD.Send(&end, 1, MPI::BOOL, 1, 0);
    cout << "processData: End signal sent to process 1\n";
    MPI::COMM_WORLD.Recv(&num, 1, MPI::INT, 2, 0);
    MPI::COMM_WORLD.Send(&end, 1, MPI::BOOL, 2, 0);
    cout << "processData: End signal sent to process 2\n";

    MPI::COMM_WORLD.Send(NULL, 0, MPI::BOOL, 3, 1);
    cout << "processData: End signal sent to process 3\n";
    MPI::COMM_WORLD.Send(NULL, 0, MPI::BOOL, 4, 1);
    cout << "processData: End signal sent to process 4\n";
    cout << "processData: END\n";
}


void finlizeData()
{
    auto rank = MPI::COMM_WORLD.Get_rank();
    int nums[20];
    int count = 0;
    for (int i = 0; i < MAX_MESSAGES; i++)
    {
        cout << "finlizeData " << rank << ": Waiting...\n";
        MPI::Status status;
        MPI::COMM_WORLD.Probe(0, MPI::ANY_TAG, status);
        int tag = status.Get_tag();

        if (tag == 0)
        {
            MPI::COMM_WORLD.Recv(&nums[count++], 1, MPI::INT, 0, tag);
            cout << "finlizeData " << rank << ": Data received: " << nums[count - 1] << endl;
        }
        else
        {
            MPI::COMM_WORLD.Recv(NULL, 0, MPI::BOOL, 0, tag);
            cout << "finlizeData " << rank << ": END\n";
            break;
        }
    }

    for (int i = 0; i < count; i++)
    {
        cout << "Procesas #" << rank << ": " << nums[i] << '\n';
    }
}


int main()
{
    MPI::Init();
    auto rank = MPI::COMM_WORLD.Get_rank();
    auto totalProcesses = MPI::COMM_WORLD.Get_size();
    cout << "thread " << rank << "/" << totalProcesses << " started.\n";

    switch (rank)
    {
    case 0:
        processData();
        break;
    case 1:
        sendData(0);
        break;
    case 2:
        sendData(11);
        break;
    case 3:
        finlizeData();
        break;
    case 4:
        finlizeData();
        break;
    }

    MPI::Finalize();
    cout << "Done\n";
}
