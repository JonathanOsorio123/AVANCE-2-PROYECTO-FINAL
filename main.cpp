#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <clocale> 
#include <algorithm> // Necesario para la funcion std::max

// ==========================================
// 1. ESTRUCTURAS DE PROYECTOS Y ESTUDIANTE
// ==========================================

struct Proyecto {
    int id_proyecto;
    std::string titulo;
    std::string descripcion;
    int anio_conclusion;
    Proyecto* siguiente; 

    Proyecto(int id, std::string t, std::string d, int anio) {
        id_proyecto = id;
        titulo = t;
        descripcion = d;
        anio_conclusion = anio;
        siguiente = nullptr;
    }
};

class Estudiante {
public:
    int id_estudiante;
    std::string nombre_completo;
    std::string carrera;
    int semestre;
    float promedio;       
    int puntaje_habilidad; // Este es el famoso 'skill_score'
    Proyecto* proyectos_cabeza;

    Estudiante() {
        id_estudiante = 0;
        semestre = 0;
        promedio = 0.0f;
        puntaje_habilidad = 0;
        proyectos_cabeza = nullptr;
    }

    Estudiante(int id, std::string nombre, std::string carr, int sem, float prom, int puntaje) {
        id_estudiante = id;
        nombre_completo = nombre;
        carrera = carr;
        semestre = sem;
        promedio = prom;
        puntaje_habilidad = puntaje;
        proyectos_cabeza = nullptr;
    }

    bool agregarProyecto(int id, std::string t, std::string d, int anio) {
        Proyecto* actual = proyectos_cabeza;
        while (actual != nullptr) {
            if (actual->id_proyecto == id) {
                return false; 
            }
            actual = actual->siguiente;
        }
        
        Proyecto* nuevo_proyecto = new Proyecto(id, t, d, anio);
        nuevo_proyecto->siguiente = proyectos_cabeza;
        proyectos_cabeza = nuevo_proyecto;
        return true;
    }

    void mostrarProyectos() {
        Proyecto* actual = proyectos_cabeza;
        if (actual == nullptr) {
            std::cout << "El estudiante no tiene proyectos registrados.\n";
            return;
        }
        while (actual != nullptr) {
            std::cout << "  - [ID Proyecto: " << actual->id_proyecto << "] " << actual->titulo 
                      << " (" << actual->anio_conclusion << "): " << actual->descripcion << "\n";
            actual = actual->siguiente;
        }
    }
};

// ==========================================
// 2. ESTRUCTURA DEL ÁRBOL AVL (Por Skill Score)
// ==========================================

struct NodoAVL {
    Estudiante* estudiante;
    NodoAVL* izquierdo;
    NodoAVL* derecho;
    int altura;

    NodoAVL(Estudiante* e) {
        estudiante = e;
        izquierdo = nullptr;
        derecho = nullptr;
        altura = 1;
    }
};

class ArbolAVL {
private:
    NodoAVL* raiz;

    int obtenerAltura(NodoAVL* n) {
        return (n == nullptr) ? 0 : n->altura;
    }

    int obtenerBalance(NodoAVL* n) {
        return (n == nullptr) ? 0 : obtenerAltura(n->izquierdo) - obtenerAltura(n->derecho);
    }

    NodoAVL* rotarDerecha(NodoAVL* y) {
        NodoAVL* x = y->izquierdo;
        NodoAVL* T2 = x->derecho;

        x->derecho = y;
        y->izquierdo = T2;

        y->altura = std::max(obtenerAltura(y->izquierdo), obtenerAltura(y->derecho)) + 1;
        x->altura = std::max(obtenerAltura(x->izquierdo), obtenerAltura(x->derecho)) + 1;

        return x;
    }

    NodoAVL* rotarIzquierda(NodoAVL* x) {
        NodoAVL* y = x->derecho;
        NodoAVL* T2 = y->izquierdo;

        y->izquierdo = x;
        x->derecho = T2;

        x->altura = std::max(obtenerAltura(x->izquierdo), obtenerAltura(x->derecho)) + 1;
        y->altura = std::max(obtenerAltura(y->izquierdo), obtenerAltura(y->derecho)) + 1;

        return y;
    }

    NodoAVL* insertarNodo(NodoAVL* nodo, Estudiante* e) {
        if (nodo == nullptr) return new NodoAVL(e);

        // Se ordena por puntaje_habilidad (skill_score)
        if (e->puntaje_habilidad < nodo->estudiante->puntaje_habilidad) {
            nodo->izquierdo = insertarNodo(nodo->izquierdo, e);
        } else {
            // Permitir duplicados o mayores a la derecha
            nodo->derecho = insertarNodo(nodo->derecho, e);
        }

        nodo->altura = 1 + std::max(obtenerAltura(nodo->izquierdo), obtenerAltura(nodo->derecho));
        int balance = obtenerBalance(nodo);

        // Caso Izquierda-Izquierda
        if (balance > 1 && e->puntaje_habilidad < nodo->izquierdo->estudiante->puntaje_habilidad) {
            return rotarDerecha(nodo);
        }
        // Caso Derecha-Derecha
        if (balance < -1 && e->puntaje_habilidad >= nodo->derecho->estudiante->puntaje_habilidad) {
            return rotarIzquierda(nodo);
        }
        // Caso Izquierda-Derecha
        if (balance > 1 && e->puntaje_habilidad >= nodo->izquierdo->estudiante->puntaje_habilidad) {
            nodo->izquierdo = rotarIzquierda(nodo->izquierdo);
            return rotarDerecha(nodo);
        }
        // Caso Derecha-Izquierda
        if (balance < -1 && e->puntaje_habilidad < nodo->derecho->estudiante->puntaje_habilidad) {
            nodo->derecho = rotarDerecha(nodo->derecho);
            return rotarIzquierda(nodo);
        }

        return nodo;
    }

    void buscarPorSkill(NodoAVL* nodo, int skill) {
        if (nodo == nullptr) return;

        // Búsqueda en preorden para listar todos los que coincidan con ese puntaje
        if (nodo->estudiante->puntaje_habilidad == skill) {
            std::cout << "  - [ID: " << nodo->estudiante->id_estudiante << "] " 
                      << nodo->estudiante->nombre_completo << " | Carrera: " << nodo->estudiante->carrera << "\n";
        }
        
        if (skill <= nodo->estudiante->puntaje_habilidad) {
            buscarPorSkill(nodo->izquierdo, skill);
        }
        if (skill >= nodo->estudiante->puntaje_habilidad) {
            buscarPorSkill(nodo->derecho, skill);
        }
    }

public:
    ArbolAVL() { raiz = nullptr; }

    void insertar(Estudiante* e) {
        raiz = insertarNodo(raiz, e);
    }

    void buscarPuntaje(int skill) {
        buscarPorSkill(raiz, skill);
    }
};

// ==========================================
// 3. ESTRUCTURA DE LA TABLA HASH
// ==========================================

struct NodoHash {
    Estudiante* estudiante;
    NodoHash* siguiente;
    
    NodoHash(Estudiante* e) {
        estudiante = e;
        siguiente = nullptr;
    }
};

class TablaHash {
private:
    static const int TAMANO_TABLA = 1009; 
    NodoHash* tabla[TAMANO_TABLA];
    int total_elementos;
    int total_colisiones;

    int funcionHash(int id) {
        return id % TAMANO_TABLA;
    }

public:
    TablaHash() {
        total_elementos = 0;
        total_colisiones = 0;
        for (int i = 0; i < TAMANO_TABLA; i++) {
            tabla[i] = nullptr;
        }
    }

    bool insertar(Estudiante* e) {
        if (e == nullptr) return false;
        
        int indice = funcionHash(e->id_estudiante);
        NodoHash* actual = tabla[indice];
        
        if (actual != nullptr) {
            total_colisiones++; 
        }
        
        while (actual != nullptr) {
            if (actual->estudiante->id_estudiante == e->id_estudiante) {
                return false; 
            }
            actual = actual->siguiente;
        }

        NodoHash* nuevo_nodo = new NodoHash(e);
        nuevo_nodo->siguiente = tabla[indice];
        tabla[indice] = nuevo_nodo;
        total_elementos++;
        return true;
    }

    Estudiante* buscar(int id) {
        int indice = funcionHash(id);
        NodoHash* actual = tabla[indice];
        
        while (actual != nullptr) {
            if (actual->estudiante->id_estudiante == id) {
                return actual->estudiante;
            }
            actual = actual->siguiente;
        }
        return nullptr; 
    }

    int obtenerTotalElementos() { return total_elementos; }
    int obtenerTotalColisiones() { return total_colisiones; }
    float obtenerFactorCarga() { return (float)total_elementos / TAMANO_TABLA; }
};

// ==========================================
// 4. ESTRUCTURA DE LA RED SOCIAL
// ==========================================

class RedSocial {
public:
    TablaHash tablaHash;
    ArbolAVL arbolAVL; // <-- ¡UNIFICADO! El arbol ahora vive dentro del sistema
    int total_proyectos_red;
    
    RedSocial() {
        total_proyectos_red = 0;
    }

    void cargarDesdeCSV(std::string nombre_archivo) {
        std::ifstream archivo(nombre_archivo);
        if (!archivo.is_open()) {
            std::cout << "Error: No se pudo abrir el archivo '" << nombre_archivo << "'\n";
            std::cout << "Verifica la ruta en tu disco D.\n";
            return;
        }

        std::string linea;
        std::getline(archivo, linea); 

        int cargados = 0;
        while (std::getline(archivo, linea)) {
            if (linea.empty()) continue;

            if (linea[linea.size() - 1] == '\r') {
                linea.erase(linea.size() - 1);
            }

            std::stringstream ss(linea);
            std::string id_str, nombre, carrera, sem_str, prom_str, puntaje_str;

            std::getline(ss, id_str, ',');
            std::getline(ss, nombre, ',');
            std::getline(ss, carrera, ',');
            std::getline(ss, sem_str, ',');
            std::getline(ss, prom_str, ',');
            std::getline(ss, puntaje_str, ',');

            if(id_str.empty()) continue;

            try {
                int id = std::stoi(id_str);
                int semestre = std::stoi(sem_str);
                float promedio = std::stof(prom_str);
                int puntaje = std::stoi(puntaje_str);

                Estudiante* nuevo = new Estudiante(id, nombre, carrera, semestre, promedio, puntaje); 

                // Al cargar, se inserta simultaneamente en ambas estructuras
                if (tablaHash.insertar(nuevo)) {
                    arbolAVL.insertar(nuevo); // <-- INTEGRACION TOTAL
                    cargados++;
                } else {
                    delete nuevo; 
                }
            } catch (...) {
                continue; 
            }
        }
        archivo.close();
        std::cout << "\n>>> EXITO: Se cargaron " << cargados << " estudiantes en la Tabla Hash y el Arbol AVL. <<<\n";
    }

    void mostrarEstadisticas() {
        std::cout << "\n================ ESTADISTICAS DEL SISTEMA ================\n";
        std::cout << "Total de estudiantes cargados: " << tablaHash.obtenerTotalElementos() << "\n";
        std::cout << "Total de colisiones Hash:       " << tablaHash.obtenerTotalColisiones() << "\n";
        std::cout << "Factor de carga de la Hash:    " << tablaHash.obtenerFactorCarga() << "\n";
        std::cout << "Total de proyectos en la red:  " << total_proyectos_red << "\n";
        std::cout << "==========================================================\n";
    }
};

// ==========================================
// 5. FUNCIÓN PRINCIPAL (MENÚ)
// ==========================================

void mostrarMenu() {
    std::cout << "\n--- MENU RED SOCIAL ACADEMICA (CON HASH Y AVL) ---\n";
    std::cout << "1. Cargar estudiantes desde CSV\n";
    std::cout << "2. Buscar estudiante por ID (Tabla Hash)\n";
    std::cout << "3. Buscar estudiantes por Skill Score (Arbol AVL)\n"; // <-- ¡NUEVA!
    std::cout << "4. Agregar proyecto a estudiante (Lista Enlazada)\n";
    std::cout << "5. Mostrar proyectos de un estudiante\n";
    std::cout << "6. Mostrar estadisticas de estructuras\n";
    std::cout << "7. Salir\n";
    std::cout << "Seleccione una opcion: ";
}

int main() {
    std::setlocale(LC_ALL, "Spanish");

    RedSocial red;
    int opcion = 0;

    do {
        mostrarMenu();
        std::cin >> opcion;
        
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "Por favor, ingrese un numero valido.\n";
            continue;
        }

        switch (opcion) {
            case 1:
                red.cargarDesdeCSV("D:\\ProyectoFinalProgra\\estudiantes.csv");
                break;
                
            case 2: {
                int id;
                std::cout << "Ingrese el ID del estudiante a buscar: ";
                std::cin >> id;
                Estudiante* e = red.tablaHash.buscar(id);
                if (e != nullptr) {
                    std::cout << "\n[Estudiante Encontrado en Tabla Hash]\n";
                    std::cout << "Nombre: " << e->nombre_completo << "\nCarrera: " << e->carrera 
                              << "\nPromedio: " << e->promedio << "\nPuntaje Skill: " << e->puntaje_habilidad << "\n";
                } else {
                    std::cout << "Estudiante con ID " << id << " no existe.\n";
                }
                break;
            }

            case 3: { // <-- Logica de busqueda en el Arbol AVL integrada
                int skill;
                std::cout << "Ingrese el Puntaje de Habilidad (Skill Score) a buscar: ";
                std::cin >> skill;
                std::cout << "\n[Resultados de Busqueda en Arbol AVL para Skill: " << skill << "]\n";
                red.arbolAVL.buscarPuntaje(skill);
                break;
            }
            
            case 4: {
                int id, id_proj, anio;
                std::string titulo, desc;
                std::cout << "Ingrese el ID del estudiante: ";
                std::cin >> id;
                Estudiante* e = red.tablaHash.buscar(id);
                if (e == nullptr) {
                    std::cout << "Estudiante no encontrado.\n";
                    break;
                }
                std::cout << "Ingrese ID del proyecto: ";
                std::cin >> id_proj;
                std::cin.ignore();
                std::cout << "Ingrese Titulo: ";
                std::getline(std::cin, titulo);
                std::cout << "Ingrese Descripcion: ";
                std::getline(std::cin, desc);
                std::cout << "Ingrese Anio de finalizacion: ";
                std::cin >> anio;

                if (e->agregarProyecto(id_proj, titulo, desc, anio)) {
                    std::cout << "Proyecto agregado con exito!\n";
                    red.total_proyectos_red++;
                } else {
                    std::cout << "Error: El ID de proyecto ya existe para este estudiante.\n";
                }
                break;
            }
            
            case 5: {
                int id;
                std::cout << "Ingrese el ID del estudiante: ";
                std::cin >> id;
                Estudiante* e = red.tablaHash.buscar(id);
                if (e != nullptr) {
                    std::cout << "\nProyectos de: " << e->nombre_completo << "\n";
                    e->mostrarProyectos();
                } else {
                    std::cout << "Estudiante no encontrado.\n";
                }
                break;
            }
            
            case 6:
                red.mostrarEstadisticas();
                break;
                
            case 7:
                std::cout << "Saliendo del sistema...\n";
                break;
                
            default:
                std::cout << "Opcion invalida.\n";
        }
        
        std::cin.clear();
        
    } while (opcion != 7);

    return 0;
}