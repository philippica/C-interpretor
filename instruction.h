#ifndef INSTRUCTION
#define INSTRUCTION




struct instruction
{
	int line;
	string instru;
	instruction(int x,string y){line = x;instru = y;};
};

class instr
{
private:
	vector <instruction> truple;
	vector <instruction> truple2;
	int Label;
	int state;
public:
	void insertInstruction(string s);
	void modify(int idx, string s);
	void init();
	void virtualOn();
	void insertInstructions(vector <instruction> s);
	vector <instruction> virtualOff();
	int curLabel();
	void print();
	instr(){Label = 0;state = 1;};
}Instru;


void instr::insertInstructions(vector <instruction> s)
{
    int l = s.size();
    for(int i = 0; i < l; i++)
    {
        insertInstruction(s[i].instru);
    }
}


vector <instruction> instr::virtualOff()
{
    state = 1;
    vector <instruction> ret;
    ret = truple2;
	while(!truple2.empty())truple2.pop_back();
	return ret;
}


void instr::modify(int idx, string s)
{
	truple[idx].instru = s;
}

int instr::curLabel()
{
	return Label;
}

void instr::virtualOn()
{
	state = 0;
}


void instr::insertInstruction(string s)
{
	instruction u(Label, s);
	if(state)
	{
		Label++;
		truple.push_back(u);
	}
	else
	{
		truple2.push_back(u);
	}
}

void instr::init()
{
	Label = 0;
	while(!truple.empty())truple.pop_back();
}

void instr::print()
{
	for(int i = 0; i < (int)truple.size(); i++)
	{
		cout<<truple[i].line<<"\t"<<truple[i].instru<<endl;
	}
	puts("");
}




#endif

