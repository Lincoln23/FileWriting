
using namespace std;
#include <iostream>
#include <fstream>
#include <limits>
#include <math.h>
#include <stdio.h>

struct Student{
	int studentID;
	int grade;
};

struct Dataset{
	int numStudents;
	Student* students;
};

struct Rejects{
	int numRejects;
	int* rejects;
};

struct Mode{
	int numModes;
	int* modes;
};

struct Statistics{
	int   minimum;
	float average;
	int   maximum;
	float popStdDev;
	float smplStdDev;
	Mode  mode;
	int   histogram[10];
};
#endif
enum position {
	SPACEBEFOREID,
	ID,
	SPACEAFTERID,
	COMMA,
	SPACEBEFOREMARK,
	MARK,
	SPACEAFTERMARK
};

int parseline(char line[], int& id, int& mark, int currentValue, position currentposition,int& negative){
	// if( line[0] != ' ' && line[0] != '\t' && line[0] < '0' && line[0] > '9'){
	// return -1;
	// }
	switch(currentposition){
		case SPACEBEFOREID:
			if(line[currentValue] == ' ' || line[currentValue] == '\t'){
				return parseline(line,id,mark,currentValue+1,SPACEBEFOREID ,negative);
			}
			if (line[currentValue] >= '0' && line[currentValue] <='9'){
				return parseline(line,id,mark,currentValue,ID, negative);
			}
			if (line[currentValue] == '+'){
				return parseline(line,id,mark,currentValue+1,SPACEBEFOREID, negative);
			}
			else{
				return -1;
			}
			break;
		case ID:
			if (line[currentValue] >= '0' && line[currentValue] <='9'){
				id*=10;
				id = id + line[currentValue]-'0';
				return parseline(line,id,mark,currentValue+1,ID, negative);
			}
			if(line[currentValue] == ' ' || line[currentValue] == '\t'){
				return parseline(line,id,mark,currentValue+1,SPACEAFTERID, negative);
			}
			if(line[currentValue] == ','){
				return parseline(line,id,mark,currentValue+1,COMMA, negative);
			}
			else{
				return -1;
			}
			break;
		case SPACEAFTERID:
			if(line[currentValue] == ' ' || line[currentValue] == '\t'){
				return parseline(line,id,mark,currentValue+1,SPACEAFTERID, negative);
			}
			if(line[currentValue] == ','){
				return parseline(line,id,mark,currentValue+1,COMMA, negative);
			}
			else{
				return -1;
			}
			break;
		case COMMA:
			if(line[currentValue] == ' ' || line[currentValue] == '\t'){
				return parseline(line,id,mark,currentValue+1,SPACEBEFOREMARK, negative);
			}
			if (line[currentValue] >= '0' && line[currentValue] <='9'){
				return parseline(line,id,mark,currentValue,MARK, negative);
			}
			if(line[currentValue] == '-'){
				negative = -1;
				return parseline(line,id,mark,currentValue+1,MARK, negative);
			}
			else{
				return -1;
			}
			break;
		case SPACEBEFOREMARK:
			if(line[currentValue] == '-'){
				negative = -1;
				return parseline(line,id,mark,currentValue+ 	1,MARK, negative);
			}
			if(line[currentValue] == ' ' || line[currentValue] == '\t'){
				return parseline(line,id,mark,currentValue+1,SPACEBEFOREMARK, negative);
			}
			if (line[currentValue] >= '0' && line[currentValue] <='9'){
				return parseline(line,id,mark,currentValue,MARK, negative);
			}
			if (line[currentValue] == '+'){
				return parseline(line,id,mark,currentValue+1,SPACEBEFOREMARK, negative);
			}
			else{
				return -1;
			}
			break;
		case MARK:
			if (line[currentValue] >= '0' && line[currentValue] <='9'){
				if(line[currentValue+1] == ' ' || line[currentValue+1] == '\t'){ // if  it is 8 7 reject it
					return -1;
				}
				mark = mark*10;
				mark = mark + line[currentValue] - '0';
				return parseline(line,id,mark,currentValue+1,MARK, negative);
			}
			if(line[currentValue] == ' ' || line[currentValue] == '\t'){
				if(negative == -1){
					mark = mark * -1;
					negative = 1;
				}
				return parseline(line,id,mark,currentValue+1,SPACEAFTERMARK, negative);
			}
			if(line[currentValue] == '\0' || line[currentValue] == '\r'){
				if(negative == -1){
					mark = mark * -1;
					negative = 1;
				}
				return 1;
			}
			else{
				return -1;
			}
			break;
		case SPACEAFTERMARK:
			if(line[currentValue] == ' ' || line[currentValue] == '\t'){
				return parseline(line,id,mark,currentValue+1,SPACEAFTERMARK, negative);
			}
			if(line[currentValue] == '\0' || line[currentValue] == '\r'){
				return 1;
			}
			else{
				return -1;
			}
			break;
	}
}

int readCSV(const char filename[],const int minAcceptableID, 
				const int maxAcceptableID,Dataset& data, 
				Rejects& rejects);

int computeStatistics(Dataset& data, Statistics& stats);

int writeCSV(const char filename[], const Statistics& stats);

bool getExtension(const char filename[], char*& extension){ // this gets the extension of the file
	bool size = false;
	int counter = 0;
	for(int i = 0; filename[i] != '\0'; i++){
		if (filename[i] == '.'){
			size = true;
		}
		else if (size == true){
			counter++;
		}
	}
	if(size == false){ // if it doesn't hit the decimal 
		return false;
	}
	bool flag = false;
	int k = 0;
	extension = new char[counter];
	int count=0;
	for(int i = 0; filename[i] != '\0'; i++){
		if (filename[i] == '.'){
			flag = true;
		}
		else if (flag == true && k<= counter){
			extension[k]=filename[i];
			k++;
		}
	}
	extension[k] = '\0';
	return flag;
}
bool compareExtension(const char* first, const char* second){
	for(int i = 0; first[i] != '\0' || second[i] != '\0'; i++){
		if(first[i] != second[i] || first[i] == '\0' || second[i] == '\0'){
			return false;
		}
	}
	return true;
}
int lengthOfFile(const char arr[]){
	int count =0;
	for (int k = 0; arr[k]!= '\0'; k++){
		count++;
	}
	return count;
}
int readCSV(const char filename[],const int minAcceptableID, 
				const int maxAcceptableID,Dataset& data, 
				Rejects& rejects){
	if (filename == NULL){
		return -1;
	}
	int maxNumErrors = 10;
	data.students = new Student[1000];
	rejects.rejects =  new int [1000];
	data.numStudents=0;
	rejects.numRejects =0;
	int correct = 0;
	int failed = 0;
	int fileLineNumber = 0;
	const int maxLineLength = 100;
	char line[maxLineLength];
	char* extension = NULL;
	char* NewFile = NULL;
	std::ifstream infile;
	
	if(!getExtension(filename, extension)){
			int length = lengthOfFile(filename);
			NewFile = new char [length+5];
			for (int i=0; i < length; i++){
				NewFile[i] = filename[i];
			}
			NewFile[length] = '.';
			NewFile[length+1] = 'c';
			NewFile[length+2] = 's';
			NewFile[length+3] = 'v';
			NewFile[length+4] = '\0';
			//rename(filename, NewFile);
			infile.open(NewFile);
	}
	else if(!compareExtension("csv", extension)){
		return -1;
	}
	else{
	infile.open(filename);
	}
	if (!infile.is_open()){
		return -1; 	
	}
	if(minAcceptableID >= maxAcceptableID){
		return -1;
	}
	delete[] extension;
	delete[] NewFile;
	bool done = false;
		while(!done){
			++fileLineNumber;
			if(!infile.getline(line,maxLineLength)){
				if(infile.eof()){
					done = true;
					break;
				}
				else{
					return -1;
				}
			}
			int id = 0;
			int negative = 1;
			int mark = 0;
			int var = parseline (line,id,mark,0,SPACEBEFOREID,negative);
			if (var == 1 && id >= minAcceptableID && id <= maxAcceptableID && mark <= 100 && mark >= 0){
				data.students[correct].studentID = id;
					data.students[correct].grade = mark;
				correct++;
				data.numStudents = correct;
			}
			else{
				rejects.rejects[failed] = fileLineNumber;
				failed++;
				rejects.numRejects = failed;
				if (failed > maxNumErrors){
					cout << failed << endl;	
					return -1;
				}
			}
		}
		if (data.numStudents == 0){
			return -1;
		}
		else if(rejects.numRejects != 0){
		 return 1;
		}
		
		else{
	return 0;
		}
}


int minfunc (Dataset& data){
	if(data.numStudents < 1){
		return std::numeric_limits<float>::quiet_NaN();
	}
	int min = data.students[1].grade;
		for (int i = 0; i < data.numStudents; i++){
			if ( min > data.students[i].grade){
				min = data.students[i].grade;
			}
		}
		return min;
}
int maxfunc(Dataset& data){
	if(data.numStudents < 1){
		return std::numeric_limits<float>::quiet_NaN();
	}
	int max = data.students[1].grade;
		for (int i = 0; i < data.numStudents; i++){
			if ( max < data.students[i].grade){
				max = data.students[i].grade;
			}
		}
		return max;	
		
}

float average(Dataset& data) {
	if(data.numStudents < 1){
		return std::numeric_limits<float>::quiet_NaN();
	}
	float counter = 0;
	float sumof = 0;
	for(int i = 0; i < data.numStudents; i ++){
		sumof += data.students[i].grade;
		counter++;
	}
	float average = sumof/counter;
	return average;

}
float smplStdDev(Dataset& data) {
	if(data.numStudents < 1){
		return std::numeric_limits<float>::quiet_NaN();
	}
	float PSD;
	int i = 0;
	float sumof = 0; 
	float squared = 0;
	float add = 0;
	float avg;
	avg = average(data);
	while ( i < data.numStudents){
		squared = pow((data.students[i].grade-avg),2);
		sumof += squared;
		i++;
		}
		sumof = sumof/(data.numStudents-1);
		sumof = sqrt(sumof);
		PSD = sumof;
		return PSD;
}

float popStdDev(Dataset& data) {
	if(data.numStudents < 1){
		return std::numeric_limits<float>::quiet_NaN();
	}
	float SSD;
	int i = 0;
	float sumof = 0; 
	float squared = 0;
	float avg = average (data);
		while ( i < data.numStudents){
			squared = pow((data.students[i].grade-avg),2);
			sumof += squared; 
			i++;
		}
		sumof = sumof/data.numStudents;
		sumof = sqrt(sumof);
		SSD = sumof;
		return sumof;
}

void selection(Dataset& data,const int size, int startIndex){
	if ( startIndex >= size - 1 ){
        return;
	}
    int minIndex = startIndex;
    for ( int i = startIndex + 1; i < size; i++ ){
        if (data.students[i].grade < data.students[minIndex].grade)
            minIndex = i;
    }
    int temp = data.students[startIndex].grade;
    data.students[startIndex].grade = data.students[minIndex].grade;
    data.students[minIndex].grade= temp;
    selection(data,data.numStudents, startIndex + 1);
}

int mode1(Dataset& data, const int size, int*& storeMode){
	if(data.numStudents < 1){
		return -1;
	}
	storeMode = new int[size];
	selection(data,size,0);
	int modecount=0;
	int count=0;
	int currentnum=data.students[0].grade;
	int index=0;
	for (int i=0;i<size;i++){ // this loops finds the modecount
		if (currentnum==data.students[i].grade){
			count++;
			if (count>modecount){
				modecount=count;
			}
		}
		else{	
			currentnum=data.students[i].grade;
			count=1;
		}
	}
	count=0;
	currentnum=data.students[0].grade;
	for (int i=0;i<size;i++){ // this loops puts the numbers int the mode[]
		if (modecount == 1){
			storeMode[index]= data.students[i].grade;
			index++;
		}
		else if (currentnum==data.students[i].grade){
			count++;
			if (count==modecount){
				storeMode[index]=currentnum;
				index++;
			}
		}
		else{
			currentnum=data.students[i].grade;
			count=1;
		}
	}
	return index;	
}	

bool calcStatisitcs(Dataset& data, int& min, int& max, float& avg,
float& pop, float& std){
	if(data.numStudents< 1){
		return false;
	}
	min = minfunc(data);
		if (min != min){
			return false;
		}
	max = maxfunc(data);
		if (max != max){
			return false;
		}
	avg = average(data);
		if (avg != avg){
			return false;
		}
	pop = popStdDev(data);
		if(pop!=pop){
				return false;
		}
	std = smplStdDev(data);
		if(std!= std){
			return false;
		}
		return true;
}	
int computeStatistics(Dataset& data, Statistics& stats){
	if(data.numStudents < 1){
		return -1;
	}
	if (!calcStatisitcs(data,stats.minimum,stats.maximum,stats.average,stats.popStdDev,stats.smplStdDev)){
		return -1;
	}
	Mode mode = {0};
	mode.modes= NULL;
	mode.numModes = mode1(data,data.numStudents,mode.modes);
	stats.mode = mode;	
	 
	for(int i = 0; i < 10; i++){
		stats.histogram[i] = 0;
	}
	for (int i = 0; i < data.numStudents; i++){
	int bucket = (data.students[i].grade)/10;
		if(data.students[i].grade == 100){
			stats.histogram[9]++;
		}
		stats.histogram[bucket]++;
	}
	return 0;
}


int writeCSV(const char filename[], const Statistics& stats){
	if(filename == NULL){
		return -1;
	}
	ofstream outfile;
	char* extension = NULL;
	char* NewFile = NULL;
	if(!getExtension(filename, extension)){ // adds the extension .stat + NULL if there isnt an extension
		int length = lengthOfFile(filename);
		NewFile = new char [length+6];
		for (int i=0; i < length; i++){
			NewFile[i] = filename[i];
		}
		NewFile[length] = '.';
		NewFile[length+1] = 's';
		NewFile[length+2] = 't';
		NewFile[length+3] = 'a';
		NewFile[length+4] = 't';
		NewFile[length+5] = '\0';
		//rename(filename, NewFile);
		outfile.open(NewFile);
	}
	else if (compareExtension(extension, "csv")) { // if it matches the extension csv, if so change it to stats
		int length = lengthOfFile(filename);
		NewFile = new char[length+1];
		for(int i =0; i < length; i++){
			NewFile[i] = filename[i];
		}
		NewFile[length-3] ='s';
		NewFile[length-2] ='t';
		NewFile[length-1] ='a';
		NewFile[length] ='t';
		NewFile[length+1] ='\0';
		//rename(filename, NewFile);
		outfile.open(NewFile);
	}
	else if (!compareExtension("stats", extension)){
		return -1;
	}
	else{
		outfile.open(filename);
	}
	if(!outfile.is_open()){
			return -1;
	}
	delete[] extension;
	delete[] NewFile;
	
	outfile << "Minimum: " << stats.minimum << "\n";
	outfile << "Average: " << stats.average << "\n";
	outfile << "Maximum: " << stats.maximum << "\n";
	outfile << "Population Standard Deviation: " << stats.popStdDev << "\n";
	outfile << "Sample Standard Deviation: " << stats.smplStdDev << "\n";
	
	if(stats.mode.numModes > 0){
		outfile << "Modes: ";
	}
	for(int i = 0; i < stats.mode.numModes; i++){
		if(i + 1 >= stats.mode.numModes){
			outfile << stats.mode.modes[i] << "\n";
		}
		else{
			outfile << stats.mode.modes[i] << ", ";
		}
	}
	outfile << "Histogram:\n";
	for(int i = 0; i < 9; i++){
		outfile << "[" << i*10 << "-" << (i+1)*10 - 1 << "]: " << stats.histogram[i] << "\n";
	}
	outfile << "[90-100]: " << stats.histogram[9];
	
	outfile.close();
	return 0;
}


int main(const int argc, const char* const argv[]) {
  	
	int minAcceptableID = 0;
	int maxAcceptableID = 2000000;

	Dataset data = {0};
	Rejects rejects = {0};
	Statistics stats = {0};

	std::cout << std::endl << "Implement some more appropriate tests in main()" << std::endl << std::endl;

	if(readCSV(argv[1], minAcceptableID, maxAcceptableID, data, rejects) < 0)
	{
		std::cout << ".csv file could not be read" << std::endl;
	}
	int ar = readCSV(argv[1], minAcceptableID, maxAcceptableID, data, rejects);
	
	cout << "here " <<ar << endl;

	if (computeStatistics(data, stats) < 0)
	{
		std::cout << "Stats could not be computed" << std::endl;
	}

	if (writeCSV(argv[1], stats) < 0)
	{
		std::cout << ".stat file could not be written" << std::endl;
	}

	std::cout << "Average: " << stats.average << std::endl;
	std::cout << "Minimum: " << stats.minimum << std::endl;
	std::cout << "Maximum: " << stats.maximum << std::endl;
	std::cout << "Population standard deviation: " << stats.popStdDev << std::endl;
	std::cout << "Sample standard deviation: " << stats.smplStdDev << std::endl;
	
	
	for (unsigned char i = 0; i < stats.mode.numModes; i++)
	{
		std::cout << "Mode: " << stats.mode.modes[i] << std::endl;
	}

	for (unsigned char i = 0; i < 10; i++)
	{
		std::cout << "Histogram bin [" << (i*10) << ", " << ((((i+1)*10) - 1) + i/9) << "]: " << stats.histogram[i] << std::endl;
	}
	std::cout << "\n\nRejected lines: ";
	for(int i = 0; i < rejects.numRejects; i++){
		std::cout << rejects.rejects[i] << ", ";
	}
	return 0;
}

