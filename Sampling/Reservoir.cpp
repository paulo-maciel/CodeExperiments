A simple solution is to create an array reservoir[] of maximum size k. 
One by one randomly select an item from stream[0..n-1]. 
If the selected item is not previously selected, then put it in reservoir[]. 
To check if an item is previously selected or not, we need to search the item in reservoir[]. 
The time complexity of this algorithm will be O(k^2). This can be costly if k is big. 

Also, this is not efficient if the input is in the form of a stream. 

It can be solved in O(n) time. The solution also suits well for input in the form of stream. 
The idea is similar to this post. Following are the steps.

1) Create an array reservoir[0..k-1] and copy first k items of stream[] to it. 
2) Now one by one consider all items from (k+1)th item to nth item. 
…a) Generate a random number from 0 to i where i is the index of the current item in stream[]. Let the generated random number is j. 
…b) If j is in range 0 to k-1, replace reservoir[j] with stream[i]

// An efficient program to randomly select
// k items from a stream of items
#include <bits/stdc++.h>
#include <time.h>
using namespace std;

// A utility function to print an array
void printArray(int stream[], int n)
{
	for (int i = 0; i < n; i++)
		cout << stream[i] << " ";
	cout << endl;
}

//The algorithm works by maintaining a reservoir of size k, 
//which initially contains the first k items of the input. 
//It then iterates over the remaining items until the input is exhausted. 
//Using one-based array indexing, let {\displaystyle i>k}i>k be the index of the 
//item currently under consideration. The algorithm then generates a random number j between 
//(and including) 1 and i. If j is at most k, then the ith item is selected and replaces 
//whichever item currently occupies the j-th position in the reservoir. Otherwise, 
//the item is discarded. In effect, for all i, the ith element of the input is chosen 
//to be included in the reservoir with probability {\displaystyle k/i}{\displaystyle k/i}. 
//Similarly, at each iteration the jth current element of the reservoir array is chosen to be 
//replaced by the new ith element with probability {\displaystyle 1/k\times k/i=1/i}{\displaystyle 1/k\times k/i=1/i}. 

//When the algorithm has finished executing, each item in the input population has equal probability 
//(i.e., k/n) of being chosen for the reservoir:

// A function to randomly select
// k items from stream[0..n-1].
void selectKItems(int stream[], int n, int k)
{
	int i; // index for elements in stream[]

	// reservoir[] is the output array. Initialize
	// it with first k elements from stream[]
	int reservoir[k];
	for (i = 0; i < k; i++)
		reservoir[i] = stream[i];

	// Use a different seed value so that we don't get
	// same result each time we run this program
	srand(time(NULL));

	// Iterate from the (k+1)th element to nth element
	for (; i < n; i++)
	{
		// Pick a random index from 0 to i.
		int j = rand() % (i + 1);

		// If the randomly picked index is smaller than k,
		// then replace the element present at the index
		// with new element from stream
		if (j < k) {
      reservoir[j] = stream[i];
    }
	}

	cout << "Following are k randomly selected items \n";
	printArray(reservoir, k);
}

// Driver Code
int main()
{
	int stream[] = {1, 2, 3, 4, 5, 6,
					7, 8, 9, 10, 11, 12};
	int n = sizeof(stream)/sizeof(stream[0]);
	int k = 5;
	selectKItems(stream, n, k);
	return 0;
}

// This is code is contributed by rathbhupendra
