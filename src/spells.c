struct hechizo {
  char tipo[20];
  int nivel;
  int componentes;
  int tiempo        // Tiempo de invocaci�n, en rondas
  int duracion;     // Tiempo de duraci�n, en rondas
  int alcance;
  (auto, enemigo, objeto, medio) objetivo;
  int salvacion;
  boolean resistencia;
};
