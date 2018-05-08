#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SFML/Graphics.hpp>


#define X 5*sin(t)
#define Y cos(t)


#define RX 1600
#define RY 900
#define PI  3.14159265358979323846
#define SCALE 50
#define SHIFTX 800
#define SHIFTY 450
#define CURVESTEP 500
#define MOVESTEP 1000
#define MAXBLINK 1000

struct k_point{
	double x,y;
	k_point(){
		x=y=0;
	}
	k_point(const double & x, const double & y){//not enough
		this->x=x;
		this->y=y;
	}
	k_point operator*(double num){
		k_point ob(x*num,y*num);
		return ob;
	}
	k_point operator+(k_point ob){
		k_point r(x+ob.x,y+ob.y);
		return r;
	}
	k_point operator-(k_point ob){
		k_point buf(x-ob.x,y-ob.y);
		return buf;
	}
	k_point operator-(){
		k_point buf(-x,-y);
		return buf;
	}
	k_point operator/(k_point ob){
		k_point buf(this->x/ob.x,this->y/ob.y);
		return buf;
	}
	k_point operator/(double ob){
		k_point buf(x/ob, y/ob);
		return buf;
	}
	k_point operator*(k_point ob){
		k_point buf(this->x * ob.x,this->y * ob.y);
		return buf;
	}
	k_point operator=(k_point ob){
		x=ob.x;
		y=ob.y;
		return (*this);
	}
	k_point operator=(double num){
		x=num;
		y=num;
		return (*this);
	}
	void out(){
		printf("%lf %lf\n",x,y);
	}
	void zero(){
		x=0;
		y=0;
	}
};


double t1=-2*PI,t2=2*PI;//множество значений параметра

//параметры сдвига
int shiftx=RX/2;
int shifty=RY/2;
//размер
int scale=SCALE;

double x(double t){
	return (X)*scale+shiftx;
}
double y(double t){
	return (Y)*scale+shifty;
}
//эти производные фальшивые, они могут быть не всегда эффективны
double dx(double t){
	double h=0.0001;
	return  (x(t+h)-x(t-h))/(2 * h);
}
double dy(double t){
	double h=0.0001;
	return  (y(t + h) - y(t - h))/ (2 * h);
}
double ddx(double t){
	double h=0.0001;
	return (x(t + h) - 2 * x(t) + x(t - h))/ (h * h);
}
double ddy(double t){
	double h=0.0001;
	return (y(t + h) - 2 * y(t) + y(t - h))/ (h * h);
}
//параметрическая
k_point curve(double t){
	k_point buf(x(t),y(t));
	return buf;
}
k_point dcurve(double t){
	k_point buf(dx(t),dy(t));
	//buf.out();
	return buf;
}
k_point ddcurve(double t){
	k_point buf(ddx(t),ddy(t));
	return buf;
}

k_point norm(k_point p){
	return p*scale/sqrt(p.x*p.x+p.y*p.y);
}
//вектор главной нормали, не нормированный
//параметр - вектор касательной
//система координат левая - поэтому векторное произведение со знаком минус
k_point gen_norm(k_point p){
	k_point buf(-p.y,p.x);
	return buf;
}

double module(k_point p){
	return sqrt(p.x*p.x+p.y*p.y);
}

double vecmul(k_point p,k_point l){
	return p.x*l.y-l.x*p.y;
}

double krivizna(double t){
	return scale*vecmul(dcurve(t),ddcurve(t))/pow(module(dcurve(t)),3);
}

k_point evoluta(double t){
	//return (curve(t)+norm(gen_norm(dcurve(t))))/abs(krivizna(t));      это было бы справдливо для натурального параметра
	double k=(pow(dx(t),2)+pow(dy(t),2))/(dx(t)*ddy(t)-ddx(t)*dy(t));
	k_point buf(x(t)-dy(t)*k,	y(t)+dx(t)*k);
	return buf;
}

void draw_points(k_point * p,int amount,sf::RenderWindow & window){
	sf::CircleShape shape(2,100);
	shape.setFillColor(sf::Color::Red);
	for (int i=0;i<amount;i++){
		shape.setPosition(p[i].x-2,p[i].y-2);
		window.draw(shape);
	}
}

void draw_line(k_point p1,k_point p2,sf::RenderWindow & window, sf::Color clr){
	sf::Vertex pts[2];
	pts[0].position = sf::Vector2f(p1.x,p1.y);
	pts[1].position = sf::Vector2f(p2.x,p2.y);
	pts[0].color=clr;
	pts[1].color=clr;
	window.draw(pts, 2, sf::Lines);
}

bool blink(k_point a,k_point b){
	if (module(a-b)>MAXBLINK) return true;
	else return false;
}

void draw_curve(sf::RenderWindow & window){
	k_point buf1,buf2=curve(t1);
	k_point e1,e2=evoluta(t1);
	double t=t1;
	double step=(t2-t1)/CURVESTEP;
	while (1){
		buf1=curve(t);
		e1=evoluta(t);
		if (!blink(buf1,buf2)) draw_line(buf1,buf2,window,sf::Color::Black);
		if (!blink(e1,e2)) draw_line(e1,e2,window,sf::Color::Green);
		buf2=buf1;
		e2=e1;
		t=t+step;
		if (t>=t2) {
			buf1=curve(t2);
			e1=evoluta(t2);
			if (!blink(buf1,buf2)) draw_line(buf1,buf2,window,sf::Color::Black);
			if (!blink(e1,e2)) draw_line(e1,e2,window,sf::Color::Green);
			break;
		}
	}
	
}
/*
void draw_evoluta(sf::RenderWindow & window){
	k_point buf1,buf2=evoluta(t1);
	double t=t1;
	double step=(t2-t1)/CURVESTEP;
	while (1){
		buf1=evoluta(t);
		draw_line(buf1,buf2,window,sf::Color::Green);
		buf2=buf1;
		t=t+step;
		if (t>=t2) {
			buf1=evoluta(t2);
			draw_line(buf1,buf2,window,sf::Color::Green);
			break;
		}
	}
}
*/


void draw_text(const char * str,sf::RenderWindow & window,sf::Color clr,int x, int y){
	sf::Font font;
	font.loadFromFile("Gascogne.ttf");
	sf::Text text;
	text.setFont(font);
	text.setString(str);
	text.setCharacterSize(50);//width 25
	text.setColor(sf::Color::Black);
	text.setPosition(x,y);
	window.draw(text);
}

void dnd(sf::RenderWindow & window,double t,char * str,int speed){
	k_point buf=evoluta(t);
	k_point buf2=curve(t);
	double rad=module(buf-buf2);
	sf::CircleShape c(rad,100);
	c.setPosition(buf.x-rad,buf.y-rad);
	c.setOutlineColor(sf::Color::Black);
	c.setOutlineThickness(1.5);
	window.clear(sf::Color::White);
	window.draw(c);
	draw_curve(window);
	draw_points(&buf,1,window);
	draw_line(curve(t),norm(dcurve(t))+curve(t),window,sf::Color::Red);
	draw_line(curve(t),curve(t)+norm(gen_norm(dcurve(t))),window,sf::Color::Magenta);
	sprintf(str,"Speed=%d  t=%lf Curvature=%lf",speed,t,krivizna(t));
	draw_text(str,window,sf::Color::Black,100,750);
	sprintf(str,"Shift x = %d ; Shift y = %d ; Scale = %d",shiftx-RX/2,shifty-RY/2,scale);
	draw_text(str,window,sf::Color::Black,100,800);
	window.display();
}

int main(){
	int turn=0;
	char str[32];
	double t=t1;
	double step=(t2-t1)/MOVESTEP;
	int speed=1;
	sf::RenderWindow window(sf::VideoMode(RX,RY),"Curves");
	window.clear(sf::Color::White);
	dnd(window,t,str,speed);
	window.display();
	while (window.isOpen()){
		sf::Event event;
		while (window.pollEvent(event)){
			if (event.type == sf::Event::Closed) window.close();//закрытие
			else if (event.type ==sf::Event:: MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left){
				int sxb=shiftx;
				int syb=shifty;
				while (!turn){
					sf::Event mv;
					while (window.pollEvent(mv)){
						if (mv.type == sf::Event::MouseMoved){
							shiftx=sxb-(event.mouseButton.x-mv.mouseMove.x);
							shifty=syb-(event.mouseButton.y-mv.mouseMove.y);
							dnd(window,t,str,speed);
						}
						else if (mv.type == sf:: Event::MouseButtonReleased){
							turn=1;
						}
					}
				}
				turn=0;
			}
			else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Right){
				if (t+step*speed<t2){//можно
					t=t+step*speed;
					dnd(window,t,str,speed);
				}//иначе нельзя
			}
			else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Left){
				if (t-step*speed>t1){//можно
					t=t-step*speed;
					dnd(window,t,str,speed);
				}//иначе нельзя
			}
			else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Up){
				if (speed<20) speed++;
				dnd(window,t,str,speed);
			}
			else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Down){
				if (speed>1) speed--;
				dnd(window,t,str,speed);
			}
			else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::W){
				shifty+=speed*SCALE/10;
				dnd(window,t,str,speed);
			}
			else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::A){
				shiftx+=speed*SCALE/10;
				dnd(window,t,str,speed);
			}
			else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::S){
				shifty-=speed*SCALE/10;
				dnd(window,t,str,speed);
			}
			else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::D){
				shiftx-=speed*SCALE/10;
				dnd(window,t,str,speed);
			}
			else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space){
				shiftx=RX/2;
				shifty=RY/2;
				dnd(window,t,str,speed);
			}
			else if (event.type == sf::Event::MouseWheelMoved){
				if (scale>3 || event.mouseWheel.delta>0) scale+=3*event.mouseWheel.delta;
				//scale*=(event.mouseWheel.delta>0?2:0.5);
				dnd(window,t,str,speed);
			}
		}
	}
	return 0;
}
