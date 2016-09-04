#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <math.h>
#include <queue>
#include <time.h>
#include <iostream>
#include <exception>
//#include "GPIOClass.h"

using namespace std;
using namespace cv;

RNG rng(12345);
#define PI 3.14159265;
int cont = 1;

int Xcentrada, Xobjeto, Yobjeto; bool victima;

#define Trian1_min    0.170616
#define Trian1_max    0.217556
#define Trian2_min    0.000284645
#define Trian2_max    0.00782807

#define oTrian1_min  0.173794
#define oTrian1_max  0.195688
#define oTrian2_min  0.0010709
#define oTrian2_max  0.00787981

vector<Point> points;

int Px;
int Py;
//------------------------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------------------------
CvPoint detecta(Mat imagen)
{
    int x = rand() % imagen.cols;
    int y = rand() % imagen.rows;
    
    return cvPoint(x, y);
}

//------------------------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------------------------
void explora(CvPoint p, Mat img, Mat regiones, int regionesActual)
{
    CvPoint actual = p;
    regiones.at<uchar>(actual) = regionesActual;
    
    int x = actual.x;
    int y = actual.y;
    queue<CvPoint> lista;
    lista.push(p);
    
    double area = 0;
    
    //momentos estadisticos
    double m10 = 0;
    double m01 = 0;
    
    double m11 = 0;
    double m20 = 0;
    double m02 = 0;
    
    //momentos centralizados
    double u20 = 0;
    double u02 = 0;
    double u11 = 0;
    
    double up20 = 0;
    double up02 = 0;
    double up11 = 0;
    
    double thetaInc = 0;
    double phi1 = 0;
    double phi2 = 0;
    
    while (!lista.empty())
    {
        area++;
        actual = lista.front();
        lista.pop();
        // Inicia expansion
        x = actual.x;
        y = actual.y;
        //Momentos estadisticos primer orden
        m10 += x;
        m01 += y;
        //momentos estadisticos segundo orden
        m11 += x*y;
        m20 += x*x;
        m02 += y*y;
        //arriba, izquierda, abajo, derecha
        Point v[4] = { Point(x, y + 1), Point(x - 1, y), Point(x, y - 1), Point(x + 1, y) };
        
        for (int i = 0; i < 4; i++)
        {
            if (v[i].x < 0)
            v[i].x = 0;
            if (v[i].x >= regiones.cols)
            v[i].x = regiones.cols - 1;
            
            if (v[i].y < 0)
            v[i].y = 0;
            if (v[i].y >= regiones.rows)
            v[i].y = regiones.rows - 1;
            
            int v1;
            
            v1 = img.at<uchar>(v[i]);
            if (v1 == 255 && regiones.at<uchar>(v[i]) == 0)
            {
                regiones.at<uchar>(v[i]) = regionesActual;
                lista.push(v[i]);
            }
        }
    }
    
    //centroides
    double xMedia = m10 / area;
    double yMedia = m01 / area;
    Point centro = Point(xMedia, yMedia);
    
    
    //Momentos centralizados
    u11 = m11 - yMedia*m10;
    u20 = m20 - xMedia*m10;
    u02 = m02 - yMedia*m01;
    
    //Momentos primo centralizados
    up11 = u11 / area;
    up20 = u20 / area;
    up02 = u02 / area;
    
    //Momentos invariantes
    double n20 = u20 / pow(area, 2);
    double n02 = u02 / pow(area, 2);
    double n11 = u11 / pow(area, 2);
    
    phi1 = n20 + n02;
    phi2 = pow((n20 - n02), 2) + (4 * pow(n11, 2));
    
    thetaInc = 0.5 * atan2((2.0*u11), (u20 - u02));
    double theta = thetaInc * (180 / 3.14159265);
    
    //---------------------Deteccion de Victimas---------------------------
    
    if(area < 61500 && (area < 31200 && area > 6000))
    {
        cout << "Area Region " << regionesActual << ": " << area << endl;
        cout << "phi1: " << phi1 << endl;
        cout << "phi2: " << phi2 << endl;
        cout << "theta: " << theta << endl;
        cout << "__________________________________________________" << endl;
        
        double minMarg = 0.90;
        double maxMarg = 1.10;
        
        if (phi1 > Trian1_min*minMarg && phi1 < Trian1_max*maxMarg && phi2 > Trian2_min*minMarg && phi2 < Trian2_max*maxMarg)
        {
            cout << "¡¡VICTIMA!!" << endl;
            cout << "Centro de victima: " << centro << endl;
            victima = true;
            
            //X y Y del objeto
            Xobjeto = centro.x;
            Yobjeto = centro.y;

            // Derecha o Izquierda segun el objeto con respecto a el centro de la imagen
            if(Xcentrada < Xobjeto)
                cout<<"Derecha"<<endl;//gpio22->setval_gpio("1");*******

            if(Xcentrada > Xobjeto)
                cout<<"Izquierda"<<endl;//gpio23->setval_gpio("1");*******

            else if (Xcentrada == Xobjeto)
            cout<<"Centro"<<endl;
            
            cout << "__________________________________________________" << endl;
        }
        
        else victima = false;
        
        int val = sqrt(u20)/100;
        int val2 = sqrt(u02)/100;
        line(regiones, (centro - Point(val*cos(thetaInc), val*sin(thetaInc))),
             (centro + Point(val*cos(thetaInc), val*sin(thetaInc))), 127, 1);
        
        line(regiones, (centro - Point(val2*cos(1.570796327 + thetaInc), val2*sin(1.570796327 + thetaInc))),
             (centro + Point(val2*cos(1.570796327 + thetaInc), val2*sin(1.570796327 + thetaInc))), 127, 1);
        
    }
    //---------------------------------------------------------------------------------------------------
    //-------------------Deteccion de obstaculos--------------------------------------
    if(area > 61500)
    {
        cout << "Area Region " << regionesActual << ": " << area << endl;
        cout << "phi1: " << phi1 << endl;
        cout << "phi2: " << phi2 << endl;
        cout << "theta: " << theta << endl;
        
        double minMarg = 0.90;
        double maxMarg = 1.10;
        
        if (phi1 > oTrian1_min*minMarg && phi1 < oTrian1_max*maxMarg && phi2 > oTrian2_min*minMarg && phi2 < oTrian2_max*maxMarg)
        {
            cout << "¡¡OBSTACULO!!" << endl;
            cout << "Centro de obstaculo: " << centro << endl;
            cout << "__________________________________________________" << endl;
        }
        
        int val = sqrt(u20)/100;
        int val2 = sqrt(u02)/100;
        line(regiones, (centro - Point(val*cos(thetaInc), val*sin(thetaInc))),
             (centro + Point(val*cos(thetaInc), val*sin(thetaInc))), 127, 1);
        
        line(regiones, (centro - Point(val2*cos(1.570796327 + thetaInc), val2*sin(1.570796327 + thetaInc))),
             (centro + Point(val2*cos(1.570796327 + thetaInc), val2*sin(1.570796327 + thetaInc))), 127, 1);
        
    }
//------------------------------------------------------------------------------------------
    cont++;
}
//------------------------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------------------------
Mat segmenta(Mat imgOriginal)
{
    
    
    Mat regiones = Mat::zeros(imgOriginal.rows, imgOriginal.cols, CV_8UC1);
    
    
    if (imgOriginal.channels() != 1)
    {
        cout << "Error: La imagen no est· en blanco y negro" << endl;
        return regiones;
    }
    
    int regionesActual = 255;
    int pixelesImagen = imgOriginal.rows*imgOriginal.cols;
    
    //Calculo de centro de la imagen en X
    Xcentrada = imgOriginal.cols/2;

    
    for (int i = 0; i < pixelesImagen*0.5; i++)
    {
        
        if (regionesActual < 0)
        break;
        
        CvPoint p = detecta(imgOriginal);
        
        int valor = 255;
        
        valor = imgOriginal.at<uchar>(p);
        
        if (valor == 255 && regiones.at<uchar>(p) == 0)
        {
            
            explora(p, imgOriginal, regiones, regionesActual);
            regionesActual -= 7;
        }
    }
    
    return regiones;
}
//------------------------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------------------------

Mat eliminacion(Mat src){

//-------------- ELIMINACION DE COLOES ESPECIFICOS ------------------------------
     
     for (int i = 0; i < src.rows; i++)
     {
        for(int k=0; k<src.cols; k++)
        {
            CvPoint p = detecta(src);
     
            Vec3b intensity = src.at<Vec3b>(p);
            float blue = intensity.val[0];
            float green = intensity.val[1];
            float red = intensity.val[2];
     
            //Deteccion de rojos solamente
            if(green > 200)
                {
                    src.at<Vec3b> (p) [0] = 0;
                    src.at<Vec3b> (p) [1] = 0;
                    src.at<Vec3b> (p) [2] = 0;
                }
        }
     }
    
    return src;
}
//------------------------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------------------------

void identificaColor(Mat vrojo){
        
    for (int i = 0; i < vrojo.rows; i++)
    {
        for(int k=0; k<vrojo.cols; k++)
        {
            CvPoint p = detecta(vrojo);
            
            if((p.x == Xobjeto)&&(p.y == Yobjeto)&&(Xobjeto != 0)&&(Yobjeto!=0))
            {
                cout<<"Mismo punto! "<< p.x <<" "<<p.y <<" | "<<Xobjeto<<" "<<Yobjeto<<endl;
                
                Vec3b intensity = vrojo.at<Vec3b>(p);
                
                float blue = intensity.val[0];
                float green = intensity.val[1];
                float red = intensity.val[2];
                cout<<"B : "<<blue<<endl;
                cout<<"G : "<<green<<endl;
                cout<<"R : "<<red<<endl;
                
                //Deteccion de rojo, amarillo o azul -----------------------------------------------------------------
                
                if(blue < 50 && green < 50)
                {
                    if((red < 190 && red > 120) && victima)
                    
                        cout<<"Color de Victima: ROJO"<<endl;//gpio4->setval_gpio("1");*****
                }
                    
                else
                if(blue < 45 && green > 150)
                {
                    if((red > 190) && victima)

                        cout<<"Color de Victima: AMARILLO"<<endl;//gpio21->setval_gpio("1");*****
                }
                else
                    if(red < 20 && green < 80)
                    {
                        if((blue > 110) && (blue <140))
                            
                            cout<<"OBSTACULO AZUL D:"<<endl;
                    }

            }
        }
    }
    
    //gpio4->setval_gpio("0");

}
//------------------------------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////////////////////////////
//------------------------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
    Mat src, vrojo, imagenThreshold, imagenHSV, sat, hue, val;
    vector<Mat> channels;
    
    /*
    string inputstate;
    GPIOClass* gpio4 = new GPIOClass("4"); //create new GPIO object to be attached to  GPIO4
    GPIOClass* gpio17 = new GPIOClass("17"); //create new GPIO object to be attached to  GPIO17
    GPIOClass* gpio21 = new GPIOClass("21"); //create new GPIO object to be attached to  GPIO21
    GPIOClass* gpio22 = new GPIOClass("22"); //create new GPIO object to be attached to  GPIO22
    GPIOClass* gpio23 = new GPIOClass("23"); //create new GPIO object to be attached to  GPIO23
    
    gpio4->export_gpio(); //export GPIO4
    gpio17->export_gpio(); //export GPIO17
    gpio21->export_gpio(); //export GPIO21
    gpio22->export_gpio(); //export GPIO22
    gpio23->export_gpio(); //export GPIO23
    
    //cout << " GPIO pins exported" << endl;
    
    gpio17->setdir_gpio("in"); //GPIO4 set to output
    gpio4->setdir_gpio("out"); // GPIO17 set to input
    gpio21->setdir_gpio("out"); // GPIO17 set to input
    gpio22->setdir_gpio("out"); // GPIO17 set to input
    gpio23->setdir_gpio("out"); // GPIO17 set to input
    
    //cout << " Set GPIO pin directions" << endl;
    
    gpio17->getval_gpio(inputstate);
    
    */
    
    int v=1;
    
    while (v==1)//inputstate == "1")
    {
        v= 0; //usleep(500);
        //gpio17->setval_gpio("0");
        
        VideoCapture camera = VideoCapture(0);
        camera.read(src);
        
        vrojo = src;
        
        int pixelesImagen = src.rows*src.cols;
        cout<<"Pixeles totales: "<<pixelesImagen<<endl;
        cout<<"COLS: "<<src.cols<<endl;
        cout<<"ROWS: "<<src.rows<<endl;
        cout<<"-------------------------------------"<<endl;
        
        eliminacion(src);
        
        imshow("IMAGEN ORIGINAL",src);
        
        cvtColor(src, imagenHSV, CV_BGR2HSV);
        
        split(imagenHSV, channels);
        
        hue = channels[0];
        sat = channels[1];
        val = channels[2];
        
        threshold(sat, sat, 150, 255, THRESH_BINARY);   // 150 -> th
        
        bitwise_not(segmenta(sat),sat);
        
        identificaColor(vrojo);
        
        imshow("SAT/G", sat);
        
        cin >> v ;
        //gpio17->getval_gpio(inputstate);
        
    }
}