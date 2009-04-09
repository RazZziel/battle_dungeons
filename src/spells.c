struct hechizo {
  char tipo[20];
  int nivel;
  int componentes;
  int tiempo        // Tiempo de invocación, en rondas
  int duracion;     // Tiempo de duración, en rondas
  int alcance;
  (auto, enemigo, objeto, medio) objetivo;
  int salvacion;
  boolean resistencia;
};
