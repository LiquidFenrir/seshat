/*Copyright 2014 Francisco Alvaro

 This file is part of SESHAT.

    SESHAT is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SESHAT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SESHAT.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "stroke.h"
#include <cfloat>
#include <cmath>
#include <algorithm>
#include <utility>

bool esNum(char c){
  return (c >= '0' && c <= '9') || c=='-' || c=='.';
}

Stroke::Stroke(int np) {
  pseq.resize(np, Punto(-1,-1));

  cx = cy = 0;
  rx = ry =  INT_MAX;
  rs = rt = -INT_MAX;
}

Stroke::Stroke(int np, FILE *fd) {
  pseq.resize(np);

  rx = ry =  INT_MAX;
  rs = rt = -INT_MAX;
  for(int i=0; i<(int)pseq.size(); i++) {
    fscanf(fd, "%f %f", &pseq[i].x, &pseq[i].y);
    if( pseq[i].x < rx ) rx = pseq[i].x;
    if( pseq[i].y < ry ) ry = pseq[i].y;
    if( pseq[i].x > rs ) rs = pseq[i].x;
    if( pseq[i].y > rt ) rt = pseq[i].y;
  }
}

Stroke::~Stroke() {
}

void Stroke::set(int idx, Punto *p) {
  pseq[idx].x = p->x;
  pseq[idx].y = p->y;

  if( pseq[idx].x < rx ) rx = pseq[idx].x;
  if( pseq[idx].y < ry ) ry = pseq[idx].y;
  if( pseq[idx].x > rs ) rs = pseq[idx].x;
  if( pseq[idx].y > rt ) rt = pseq[idx].y;
}

Punto *Stroke::get(int idx) {
  return &pseq[idx];
}

const Punto *Stroke::get(int idx) const {
  return &pseq[idx];
}

int Stroke::getNpuntos() const {
  return (int)pseq.size();
}

void Stroke::print() {
  printf("STROKE - %d points\n", (int)pseq.size());
  for(int i=0; i<(int)pseq.size(); i++)
    printf(" (%g,%g)", pseq[i].x, pseq[i].y);
  printf("\n");
}

float Stroke::min_dist(Stroke *st) {
  float mind = FLT_MAX;
  for(int i=0; i<(int)pseq.size(); i++) {
    for(int j=0; j<st->getNpuntos(); j++) {
      Punto *p = st->get(j);

      float d = (pseq[i].x - p->x)*(pseq[i].x - p->x)
	+ (pseq[i].y - p->y)*(pseq[i].y - p->y);

      if( d < mind ) mind=d;
    }
  }

  return sqrt( mind );
}
