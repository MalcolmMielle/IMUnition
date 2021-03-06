#include "Graph3D.hpp"

void Graph3D::draw(Environnement* env){
	drawRepere();
}

void Graph3D::draw(IMU* cap){
	//Save the matrice ModelView avant de la modifier
	glPushMatrix();
	//Modèle en quaternion afin d'éviter les angles probématiques d'Euler.
	Quaternion myQuaternion(cap->getRealroll(), cap->getRealpitch(), cap->getRealyaw());
	matModel=myQuaternion.getMatrix();
	glMultMatrixf(matModel.getTranspose());
	
	/********************************************
	Rotation en fonction des pitch yaw et roll du capteur
	glRotate = Rotation de angle autour d'un vecteur
	L'avant du capteur est considéré en Y
	Donc pitch fais pencher le capteur sur le côté donc il est selon x
	Roll fais tourner le capteur autour de son axe principal donc y
	Yaw le fais bouger "à plat" donc selon z*/	
	//Equivalent en openGL	
	//glRotatef(cap->getPitch(), 1,0,0);
	//glRotatef(cap->getRoll(), 0,1,0);
	//glRotatef(cap->getYaw(), 0,0,1);
	//Puis on dessine
	/*********************************************/
	glBegin(GL_POLYGON);
		glColor3ub(200,50,0);//bleu
		glVertex3f(-1,-1,0);
		glVertex3f(1,-1,0);
		glVertex3f(1,1,0);
		glVertex3f(-1,1,0);
	glEnd();
	
	glBegin(GL_LINE_LOOP);
		glColor3ub(255,255,255);//blanc
		glVertex3f(0.3,1,0);
		glVertex3f(0.3,1.5,0);
		glVertex3f(1,1.5,0);
		glVertex3f(0,2,0);
		glVertex3f(-1,1.5,0);
		glVertex3f(-0.3,1.5,0);
		glVertex3f(-0.3,1,0);
	glEnd();
	/**********************************Dessin de l'image echographique*********************************/
	//On ne reinitialise pas la matrice de dession d'openGL qui est enregistrer. Par contre, on continu de faire bouger la matrice matModel définissant la position de notre image. Comme ça on fait : Deplace de la matrice à l'orientation du capteur -> Dessin du capteur -> Deplacement de la matrice jusqu'à la cible échographique -> Dessin de l'image d'arrivé
	myQuaternion=
	imgecho.getQua();
	matModel=myQuaternion.getMatrix();
	matModel=matModel.translate(imgecho.getVec());
	glMultMatrixf(matModel.getTranspose());
	drawRepere();
	glPopMatrix();	
	
	
	/***********************************Affiche les donnes du capteurs**********************************/
	glLoadIdentity();                   // reset modelview matrix
	drawInfoImu(cap);
	//glMatrixMode(GL_PROJECTION); //Définit le type de la pile de matrice
	
	/*****************************END*******************************************************************/
	glutSwapBuffers();
}

void Graph3D::drawInfoImu(IMU* cap){
	// set to 2D orthogonal projection
	glMatrixMode(GL_PROJECTION);        // switch to projection matrix
	glPushMatrix();                     // save current projection matrix
	glLoadIdentity();                   // reset projection matrix
	gluOrtho2D(0, 600, 0, 600); // set to orthogonal projection

	float color[4] = {1, 1, 1, 1};

	std::stringstream ss;
	ss << std::fixed << std::setprecision(3);

	drawString("=== IMU ===", 0, 600-13, color, font);
	drawString("=== RAW Accelerometre===", 0, 600-(3*13), color, font);
	ss << "[" << std::setw(8) << cap->getRollA() << std::setw(8) << cap->getPitchA() << std::setw(8) << cap->getYawA() <<  "]" << std::endl;
	drawString(ss.str().c_str(), 0, 600-(6*13), color, font);
	ss.str("");
	
	drawString("=== RAW Gyroscope===", 0, 600-(9*13), color, font);
	ss << "[" << std::setw(8) << cap->getRollG() << std::setw(8) << cap->getPitchG() << std::setw(8) << cap->getYawG() <<  "]" << std::endl;
	drawString(ss.str().c_str(), 0, 600-(12*13), color, font);
	ss.str("");
	
	/*****Dessine les infos des autres capteurs***/
	drawString("=== KALMAN ===", 0, 600-(15*13), color, font);
	ss << "[" << std::setw(8) << cap->getRollK() << std::setw(8) << cap->getPitchK() << std::setw(8) << cap->getYawK() <<  "]" << std::endl;
	drawString(ss.str().c_str(), 0, 600-(18*13), color, font);
	ss.str("");
	
	
	drawString("=== COMPENSATEUR ===", 0, 600-(21*13), color, font);
	ss << "[" << std::setw(8) << cap->getRollC() << std::setw(8) << cap->getPitchC() << std::setw(8) << cap->getYawC() <<  "]" << std::endl;
	drawString(ss.str().c_str(), 0, 600-(24*13), color, font);
	ss.str("");
	

	drawString("=== Magnéto, Température, Pression ===", 0, 600-(27*13), color, font);
	ss << "[" << std::setw(8) << cap->getMag() << std::setw(8) << cap->getTemp() << std::setw(16) << cap->getPress() <<  "]" << std::endl;
	drawString(ss.str().c_str(), 0, 600-(30*13), color, font);
	ss.str("");
	
	drawString("=== NOW ===", 0, 600-(33*13), color, font);
	ss << "[" << std::setw(8) << cap->getRealroll() << std::setw(8) << cap->getRealpitch() << std::setw(16) << cap->getRealyaw() <<  "]" << std::endl;
	drawString(ss.str().c_str(), 0, 600-(36*13), color, font);
	ss.str("");
	
	
	glPopMatrix();                   // restore to previous projection matrix
    	// restore modelview matrix
    	glMatrixMode(GL_MODELVIEW);      // switch to modelview matrix
    	glPopMatrix();
}

void Graph3D::drawString(const char *str, int x, int y, float color[4], void *font){
	glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT); // lighting and color mask
	glDisable(GL_LIGHTING);     // need to disable lighting for proper text color
	glDisable(GL_TEXTURE_2D);

	glColor4fv(color);          // set text color
	glRasterPos2i(x, y);        // place text position

	// loop all characters in the string
	while(*str)
	{
	  glutBitmapCharacter(font, *str);
	  ++str;
	}

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	glPopAttrib();
}


void Graph3D::drawRepere(){
	glPushMatrix();
	glLineWidth(3);
	glBegin(GL_LINES);
		glColor3ub(0,255,0); //Parce qu'on utilise la bibliothèque Glu
	  	glVertex3f(0,0,0);
	  	glVertex3f(0,0,3);
	  	glColor3ub(255,0,0); 
	  	glVertex3f(0,0,0);
	  	glVertex3f(0,3,0);
	  	glColor3ub(0,0,255); 
	  	glVertex3f(0,0,0);
	  	glVertex3f(3,0,0);
		
	glEnd();
	glLineWidth(1);
	glFlush();
	glPopMatrix();
}
