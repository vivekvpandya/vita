#include <iostream>
#include <map>
#include <string>
#include <ctime>
#include <cstring>

using namespace std;

typedef struct news {
    char timeStamp[30];
    char title[100];
    char details[500];
} News ;
typedef map<string, News> NewsMap;
typedef map<string, News>::iterator NmapIt;

NewsMap newsMap;

double compareTS(char *ts1, char *ts2) {
	struct tm time1, time2;
    strptime(ts1,"%a %b %e %T %Z %Y",&time1);
    strptime(ts2,"%a %b %e %T %Z %Y",&time2);
    time_t time1t = mktime(&time1);
    time_t time2t = mktime(&time2);
    double seconds = difftime(time1t, time2t);
    return seconds;
}

bool updateNews(News news) {
	bool returnVal = false; // false => no match found so just inserted the new entry or match found but argument is older news so no need to update
	//string key(news.title);
	NmapIt found = newsMap.find(news.title);
	if(found != newsMap.end()) {
		if (compareTS(found->second.timeStamp, news.timeStamp) < 0) {
			newsMap[news.title] = news;
			returnVal = true; // true => a match found and updated the old value
		}
	} 
	return returnVal;

}

int main(int argc, char **argv) {
	News n1;
	strncpy(n1.timeStamp,"Thu Mar  3 22:50:41 IST 2016",29); 
	strncpy(n1.title,"Sachin scores 200",99); 
	strncpy(n1.details,"Yeah, He does that.",499);
	
	News n2;
	strncpy(n2.timeStamp,"Thu Mar  3 22:50:45 IST 2016",29); 
	strncpy(n2.title,"Sachin scores 200",99); 
	strncpy(n2.details,"Yeah, He does that. This should be updated one.",499);

	
	newsMap[n1.title] = n1;

	if(updateNews(n2)) {
		NmapIt found = newsMap.find(n1.title);
			if(found != newsMap.end()) {
				cout << found->second.details << "\n";
			}
	}

	return 0;
}



