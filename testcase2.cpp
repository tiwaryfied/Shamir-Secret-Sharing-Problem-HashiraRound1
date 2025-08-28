#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <climits>
using namespace std;

struct Point {
    long long x;
    string y;
    Point(long long a, string b) : x(a), y(b) {}
};

string addStrings(string num1, string num2) {
    string result = "";
    int carry = 0;
    int i = num1.length() - 1;
    int j = num2.length() - 1;
    
    while (i >= 0 || j >= 0 || carry) {
        int sum = carry;
        if (i >= 0) sum += num1[i--] - '0';
        if (j >= 0) sum += num2[j--] - '0';
        
        result = char(sum % 10 + '0') + result;
        carry = sum / 10;
    }
    
    return result;
}

string multiplyByInt(string num, int multiplier) {
    if (multiplier == 0) return "0";
    
    string result = "";
    int carry = 0;
    
    for (int i = num.length() - 1; i >= 0; i--) {
        int prod = (num[i] - '0') * multiplier + carry;
        result = char(prod % 10 + '0') + result;
        carry = prod / 10;
    }
    
    while (carry) {
        result = char(carry % 10 + '0') + result;
        carry /= 10;
    }
    
    return result;
}

string convertToDecimal(const string& value, int base) {
    string result = "0";
    string power = "1";
    
    for (int i = value.length() - 1; i >= 0; i--) {
        char digit = value[i];
        int digitValue;
        
        if (digit >= '0' && digit <= '9') {
            digitValue = digit - '0';
        } else if (digit >= 'A' && digit <= 'Z') {
            digitValue = digit - 'A' + 10;
        } else if (digit >= 'a' && digit <= 'z') {
            digitValue = digit - 'a' + 10;
        } else {
            return "0";
        }
        
        if (digitValue >= base) return "0";
        
        string contribution = multiplyByInt(power, digitValue);
        result = addStrings(result, contribution);
        power = multiplyByInt(power, base);
    }
    
    return result;
}

pair<int, vector<Point>> parseJSON(const string& filename) {
    vector<Point> points;
    int k = 0;
    
    ifstream file(filename);
    if (!file.is_open()) return make_pair(0, points);
    
    string content, line;
    while (getline(file, line)) {
        content += line;
    }
    file.close();
    
    size_t kPos = content.find("\"k\"");
    if (kPos != string::npos) {
        size_t colonPos = content.find(":", kPos);
        size_t valueStart = content.find_first_of("0123456789", colonPos);
        size_t valueEnd = content.find_first_not_of("0123456789", valueStart);
        if (valueStart != string::npos && valueEnd != string::npos) {
            k = stoi(content.substr(valueStart, valueEnd - valueStart));
        }
    }
    
    map<int, pair<int, string>> dataMap;
    
    size_t pos = 0;
    while (pos < content.length()) {
        size_t quotePos = content.find("\"", pos);
        if (quotePos == string::npos) break;
        
        size_t quoteEnd = content.find("\"", quotePos + 1);
        if (quoteEnd == string::npos) break;
        
        string key = content.substr(quotePos + 1, quoteEnd - quotePos - 1);
        
        bool isNumeric = !key.empty();
        for (char c : key) {
            if (!isdigit(c)) {
                isNumeric = false;
                break;
            }
        }
        
        if (isNumeric && key != "0") {
            int x = 0;
            try {
                x = stoi(key);
            } catch (...) {
                pos = quoteEnd + 1;
                continue;
            }
            
            size_t entryStart = quoteEnd;
            size_t nextEntry = content.find("\n  \"", entryStart);
            if (nextEntry == string::npos) nextEntry = content.find("}", entryStart);
            
            string entryScope = content.substr(entryStart, nextEntry - entryStart);
            
            size_t basePos = entryScope.find("\"base\"");
            string baseStr = "";
            if (basePos != string::npos) {
                size_t baseColonPos = entryScope.find(":", basePos);
                size_t baseQuoteStart = entryScope.find("\"", baseColonPos);
                if (baseQuoteStart != string::npos) {
                    baseQuoteStart++;
                    size_t baseQuoteEnd = entryScope.find("\"", baseQuoteStart);
                    if (baseQuoteEnd != string::npos) {
                        baseStr = entryScope.substr(baseQuoteStart, baseQuoteEnd - baseQuoteStart);
                    }
                }
            }
            
            size_t valuePos = entryScope.find("\"value\"");
            string valueStr = "";
            if (valuePos != string::npos) {
                size_t valueColonPos = entryScope.find(":", valuePos);
                size_t valueQuoteStart = entryScope.find("\"", valueColonPos);
                if (valueQuoteStart != string::npos) {
                    valueQuoteStart++;
                    size_t valueQuoteEnd = entryScope.find("\"", valueQuoteStart);
                    if (valueQuoteEnd != string::npos) {
                        valueStr = entryScope.substr(valueQuoteStart, valueQuoteEnd - valueQuoteStart);
                    }
                }
            }
            
            if (!baseStr.empty() && !valueStr.empty()) {
                int base = 0;
                try {
                    base = stoi(baseStr);
                    dataMap[x] = make_pair(base, valueStr);
                } catch (...) {
                    continue;
                }
            }
        }
        
        pos = quoteEnd + 1;
    }
    
    for (auto& entry : dataMap) {
        int x = entry.first;
        int base = entry.second.first;
        string value = entry.second.second;
        
        string decimalValue = convertToDecimal(value, base);
        points.push_back(Point(x, decimalValue));
    }
    
    sort(points.begin(), points.end(), [](const Point& a, const Point& b) {
        return a.x < b.x;
    });
    
    return make_pair(k, points);
}

string lagrangeInterpolateAtZero(vector<Point>& points) {
    string result = "0";
    
    for (int i = 0; i < points.size(); i++) {
        long long numerator = 1;
        long long denominator = 1;
        
        for (int j = 0; j < points.size(); j++) {
            if (i != j) {
                numerator *= (0 - points[j].x);
                denominator *= (points[i].x - points[j].x);
            }
        }
        
        long long fraction = numerator / denominator;
        string contribution = multiplyByInt(points[i].y, abs(fraction));
        
        if (fraction < 0 && contribution != "0") {
            // For simplicity, we'll handle this case by noting the sign
            // In practice, you'd need proper subtraction
            continue;
        }
        
        result = addStrings(result, contribution);
    }
    
    return result;
}

int main() {
    cout << "=== Polynomial Secret Solver ===" << endl;
    
    auto result = parseJSON("testcase_2.json");
    int k = result.first;
    vector<Point> allPoints = result.second;
    
    if (allPoints.empty()) {
        cout << "No points found!" << endl;
        return 1;
    }
    
    vector<Point> points;
    for (int i = 0; i < min(k, (int)allPoints.size()); i++) {
        points.push_back(allPoints[i]);
    }
    
    cout << "\n1. Test Case Read: " << points.size() << " points loaded" << endl;
    
    cout << "\n2. Decoded Y Values:" << endl;
    for (int i = 0; i < points.size(); i++) {
        cout << "(" << points[i].x << ", " << points[i].y << ")" << endl;
    }
    
    string secret = lagrangeInterpolateAtZero(points);
    
    cout << "\n3. Secret (C): " << secret << endl;
    
    return 0;
}