#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <cmath>
#include <map>
#include <algorithm>
using namespace std;

struct Point {
    double x, y;
    Point(double a, double b) : x(a), y(b) {}
};

// Convert the number from given base to decimal
long long convertToDecimal(const string& value, int base) {
    long long result = 0;
    long long power = 1;
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
            cout << "Invalid character in number: " << digit << endl;
            return 0;
        }
        
        if (digitValue >= base) {
            cout << "Invalid digit " << digit << " for base " << base << endl;
            return 0;
        }
        
        result += digitValue * power;
        power *= base;
    }
    
    return result;
}

string extractQuotedValue(const string& content, size_t startPos, const string& key) {
    size_t keyPos = content.find("\"" + key + "\"", startPos);
    if (keyPos == string::npos) return "";
    
    size_t colonPos = content.find(":", keyPos);
    if (colonPos == string::npos) return "";
    
    size_t quoteStart = content.find("\"", colonPos);
    if (quoteStart == string::npos) return "";
    quoteStart++;
    
    size_t quoteEnd = content.find("\"", quoteStart);
    if (quoteEnd == string::npos) return "";
    
    return content.substr(quoteStart, quoteEnd - quoteStart);
}

pair<int, vector<Point>> parseJSON(const string& filename) {
    vector<Point> points;
    int k = 0;
    
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error: Could not open file " << filename << endl;
        return make_pair(0, points);
    }
    
    string content = "";
    string line;
    while (getline(file, line)) {
        content += line;
    }
    file.close();
    
    cout << "JSON Content:\n" << content << "\n" << endl;
    
    // Extract k value
    size_t kPos = content.find("\"k\"");
    if (kPos != string::npos) {
        size_t colonPos = content.find(":", kPos);
        size_t valueStart = content.find_first_of("0123456789", colonPos);
        size_t valueEnd = content.find_first_not_of("0123456789", valueStart);
        if (valueStart != string::npos && valueEnd != string::npos) {
            k = stoi(content.substr(valueStart, valueEnd - valueStart));
        }
    }
    
    cout << "k = " << k << endl;
    
    map<int, pair<int, string>> dataMap; // x -> (base, value)
    
    size_t pos = 0;
    while (pos < content.length()) {
        size_t quotePos = content.find("\"", pos);
        if (quotePos == string::npos) break;
        
        size_t quoteEnd = content.find("\"", quotePos + 1);
        if (quoteEnd == string::npos) break;
        
        string key = content.substr(quotePos + 1, quoteEnd - quotePos - 1);
        
        bool isNumeric = true;
        if (key.empty()) isNumeric = false;
        for (char c : key) {
            if (!isdigit(c)) {
                isNumeric = false;
                break;
            }
        }
        
        if (isNumeric && key != "0") {
            int x = stoi(key);
            
            size_t entryStart = quoteEnd;
            size_t nextEntry = content.find("\n    \"", entryStart);
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
                int base = stoi(baseStr);
                dataMap[x] = make_pair(base, valueStr);
                cout << "Found entry: x=" << x << ", base=" << base << ", value=" << valueStr << endl;
            }
        }
        
        pos = quoteEnd + 1;
    }
    
    for (auto& entry : dataMap) {
        int x = entry.first;
        int base = entry.second.first;
        string value = entry.second.second;
        
        long long decimalValue = convertToDecimal(value, base);
        points.push_back(Point(x, static_cast<double>(decimalValue)));
        
        cout << "Point: x=" << x << ", y=" << decimalValue << " (from base " << base << " value " << value << ")" << endl;
    }
    
    sort(points.begin(), points.end(), [](const Point& a, const Point& b) {
        return a.x < b.x;
    });
    
    return make_pair(k, points);
}
double lagrangeBasis(int i, double x, vector<Point>& points) {
    double result = 1.0;
    for (int j = 0; j < points.size(); j++) {
        if (i != j) {
            result *= (x - points[j].x) / (points[i].x - points[j].x);
        }
    }
    return result;
}

double interpolate(double x, vector<Point>& points) {
    double result = 0.0;
    for (int i = 0; i < points.size(); i++) {
        result += points[i].y * lagrangeBasis(i, x, points);
    }
    return result;
}

int main()
{
    
    cout << "\nStep 1: Reading JSON file..." << endl;
    auto result = parseJSON("testcase_1.json");
    int k = result.first;
    vector<Point> allPoints = result.second;
    
    if (allPoints.empty()) {
        cout << "No valid points found in JSON file!" << endl;
        return 1;
    }
    
    vector<Point> points;
    for (int i = 0; i < min(k, (int)allPoints.size()); i++) {
        points.push_back(allPoints[i]);
    }
    
    cout << "\nUsing k = " << k << " points:" << endl;
    for (int i = 0; i < points.size(); i++) {
        cout << "Point " << i+1 << ": (" << points[i].x << ", " << points[i].y << ")" << endl;
    }
    
    cout << "\nStep 2: Applying Lagrange Interpolation" << endl;
    cout << "Polynomial degree: " << points.size() - 1 << endl;
    
    double constantTerm = interpolate(0, points);
    
    cout << "\nStep 3: Finding the constant term (secret):" << endl;
    cout << "======================" << endl;
    cout << "f(0) = " << constantTerm << endl;
    cout << "======================" << endl;
    
    cout << "\n=== Verification ===" << endl;
    cout << "Testing Lagrange interpolation with original points:" << endl;
    
    for (int i = 0; i < points.size(); i++) {
        double x_val = points[i].x;
        double expected_y = points[i].y;
        double calculated_y = interpolate(x_val, points);
        
        cout << "x = " << x_val << ": expected = " << expected_y << ", calculated = " << calculated_y;
        if (abs(calculated_y - expected_y) < 0.001) {
            cout << " ✓" << endl;
        } else {
            cout << " ✗" << endl;
        }
    }
    
    cout << "\n=== FINAL ANSWER ===" << endl;
    cout << "The secret (constant term) is: " << constantTerm << endl;
    
    return 0;
}

/******************************************************************************
So it is a Polynomial Equation Solver using Lagrange Interpolation
Reads test cases from JSON file with base conversion support
Fixed to properly handle k parameter and JSON parsing
*******************************************************************************/