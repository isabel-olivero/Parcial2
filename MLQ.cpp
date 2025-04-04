#include <iostream>
#include <queue>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>
using namespace std;

class Process {
    private:
        int arrivalTime, BrushTime, cola, waitingTime, turnAroundTime, completeTime, priority, runTime;
        char id;
        int originalBT;
    public:
        Process(char i, int ar, int b, int c, int p) {
            id = i;
            arrivalTime = ar;
            BrushTime = b;
            originalBT = b;
            cola = c;
            priority = p;
            completeTime = 0;
            waitingTime = 0;
            turnAroundTime = 0;
            runTime = -1;
        }
        int getWait() { return completeTime - arrivalTime - originalBT; }
        int getTat() { return completeTime - arrivalTime; }
        int getBrush() { return BrushTime; }
        int getOriginalBT() { return originalBT; }
        char getid() { return id; }
        int getRun() { return runTime; }
        int getCT() { return completeTime; }
        int getP() { return priority; }
        int getCola() { return cola; }
        int getArrivalTime() { return arrivalTime; }
        void actRun(int a) { if(runTime == -1) runTime = a; }
        void actBrush(int a) { BrushTime = (BrushTime <= a) ? 0 : BrushTime - a; }
        void actCT(int a) { completeTime = a; turnAroundTime = completeTime - arrivalTime; waitingTime = turnAroundTime - originalBT; }
};

class Queue {
    private:
        queue<Process> procesos;
        queue<Process> finalizado;
        bool roundRobin;
        int quantum;
        int Tfin;
    public:
        Queue(bool r, int q) { roundRobin = r; quantum = q; Tfin = 0; }
        void agregarProceso(Process p) { procesos.push(p); }
        bool getR() { return roundRobin; }
        void ejecutar(int time) { roundRobin ? RR(time) : FCFS(time); }
        int getTime() { return Tfin; }
        queue<Process> getFin() { return finalizado; }
        void RR(int time) {
            int time2 = time;
            while (!procesos.empty()) {
                Process p = procesos.front();
                procesos.pop();
                p.actRun(time2);
                int execTime = min(p.getBrush(), quantum);
                p.actBrush(quantum);
                time2 += execTime;
                if(p.getBrush() == 0) {
                    p.actCT(time2);
                    finalizado.push(p);
                } else {
                    procesos.push(p);
                }
            }
            Tfin = time2;
        }
        void FCFS(int time) {
            int time2 = time;
            while (!procesos.empty()) {
                Process p = procesos.front();
                procesos.pop();
                p.actRun(time2);
                int execTime = p.getBrush();
                time2 += execTime;
                p.actCT(time2);
                finalizado.push(p);
            }
            Tfin = time2;
        }
};

class MLQ {
    private:
        vector<Queue> colas;
        int wt, tat, ct, rt;
    public:
        MLQ(int a) {}
        void agregarColas(Queue p) { colas.push_back(p); }
        void ejecutar() {
            int time = 0;
            for(auto &act : colas) {
                act.ejecutar(time);
                time = act.getTime();
            }
        }
        void promedios(ostream &out) {
            int wt1 = 0, tat1 = 0, ct1 = 0, rt1 = 0, n = 0;
            for(auto &act : colas) {
                queue<Process> fin = act.getFin();
                while(!fin.empty()) {
                    Process p = fin.front();
                    fin.pop();
                    wt1 += p.getWait();
                    tat1 += p.getTat();
                    ct1 += p.getCT();
                    rt1 += p.getRun();
                    n++;
                }
            }
            if(n > 0)
                out << "WT=" << wt1/n << ";CT=" << ct1/n << ";RT=" << rt1/n << ";TAT=" << tat1/n << endl;
            else
                out << "WT=0;CT=0;RT=0;TAT=0" << endl;
        }
        void mostrar(ostream &out) {
            for(auto &act : colas) {
                queue<Process> fin = act.getFin();
                while(!fin.empty()) {
                    Process p = fin.front();
                    fin.pop();
                    out << p.getid() << ";" << p.getOriginalBT() << ";" << p.getArrivalTime() << ";" << p.getCola() << ";" << p.getP()
                        << ";" << p.getWait() << ";" << p.getCT() << ";" << p.getRun() << ";" << p.getTat() << endl;
                }
            }
        }
};

int main(int argc, char* argv[]) {
    if(argc < 2) {
        cerr << "Uso: " << argv[0] << " <archivo_de_entrada>" << endl;
        return 1;
    }
    ifstream archivo(argv[1]);
    if(!archivo) {
        cerr << "Error al abrir el archivo: " << argv[1] << endl;
        return 1;
    }
    Queue RR1(true, 3), RR2(true, 5), fcfs(false, 0);
    string linea;
    while(getline(archivo, linea)) {
        if(linea.empty()) continue;
        stringstream ss(linea);
        char etiqueta; int bt, at, q, pr; char sep;
        if(!(ss >> etiqueta >> sep >> bt >> sep >> at >> sep >> q >> sep >> pr)) {
            cerr << "Error en el formato de la linea: " << linea << endl;
            continue;
        }
        Process proceso(etiqueta, at, bt, q, pr);
        if(q == 1)
            RR1.agregarProceso(proceso);
        else if(q == 2)
            RR2.agregarProceso(proceso);
        else
            fcfs.agregarProceso(proceso);
    }
    archivo.close();
    MLQ scheduling(0);
    scheduling.agregarColas(RR1);
    scheduling.agregarColas(RR2);
    scheduling.agregarColas(fcfs);
    scheduling.ejecutar();
    ofstream out("salida.txt");
    scheduling.mostrar(out);
    scheduling.promedios(out);
    out.close();
    return 0;
}
