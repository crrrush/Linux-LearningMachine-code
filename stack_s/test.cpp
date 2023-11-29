#include<iostream>
using namespace std;
#include"st_s.hpp"



namespace simulation
{
	void test1()
	{
		stack<char> st;
		cout << st.empty() << endl;

		st.push('a');
		st.push('b');
		st.push('c');
		st.push('d');

		cout << st.size() << endl;
		cout << st.top() << endl;

		st.pop();
		st.pop();

		cout << st.size() << endl;
		cout << st.top() << endl;

	}
}



int main()
{
	simulation::test1();


	return 0;
}
