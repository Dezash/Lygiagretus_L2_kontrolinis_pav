// K2_pavyzdine.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <mpi.h>

using namespace std;

const int MAX_MESSAGES = 20;

void sendData(int start)
{
    for (int i = start; i < start + MAX_MESSAGES + 1; i++)
    {
        int num = i;
        MPI::COMM_WORLD.Send(&num, 1, MPI::INT, 0, 0);

        bool isFinished = false;
        MPI::COMM_WORLD.Recv(&isFinished, 1, MPI::BOOL, 0, 0);

        if (isFinished)
            return;
    }
}


void processData()
{
    int totalProcessed = 0;

    for (int i = 0; i < MAX_MESSAGES; i++)
    {
        int num;
        MPI::COMM_WORLD.Recv(&num, 1, MPI::INT, MPI::ANY_SOURCE, 0);

        MPI::COMM_WORLD.Send(&num, 1, MPI::INT, num % 2 == 0 ? 3 : 4, 0);
    }

    bool end = true;
    int num;
    MPI::COMM_WORLD.Recv(&num, 1, MPI::INT, 1, 0);
    MPI::COMM_WORLD.Send(&end, 1, MPI::BOOL, 1, 0);
    MPI::COMM_WORLD.Recv(&num, 1, MPI::INT, 1, 0);
    MPI::COMM_WORLD.Send(&end, 1, MPI::BOOL, 1, 0);

    MPI::COMM_WORLD.Send(NULL, 0, MPI::BOOL, 3, 1);
    MPI::COMM_WORLD.Send(NULL, 0, MPI::BOOL, 4, 1);
}


void finlizeData()
{
    int nums[20];
    int count = 0;
    for (int i = 0; i < MAX_MESSAGES; i++)
    {
        MPI::Status status;
        MPI::COMM_WORLD.Probe(0, MPI::ANY_TAG, status);
        int tag = status.Get_tag();

        if (tag == 0)
        {
            MPI::COMM_WORLD.Recv(&nums[count++], 1, MPI::INT, 0, tag);
        }
        else
        {
            MPI::COMM_WORLD.Recv(NULL, 0, MPI::BOOL, 0, tag);
            break;
        }
    }

    auto rank = MPI::COMM_WORLD.Get_rank();
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
    case 4:
        // Worker thread
        finlizeData();
        break;
    }


    MPI::Finalize();
}
