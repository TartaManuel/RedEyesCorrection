// OpenCVApplication.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "common.h"
#include "math.h"
#include <queue>
#include <random>
using namespace std;

# define M_PI 3.14159265358979323846

int diAnteriori[4] = { -1, -1, -1,  0 };
int djAnteriori[4] = { -1,  0,  1, -1 };

int diContur8Lab5[8] = { -1, -1, -1,  0, 0,  1, 1, 1 };
int djContur8Lab5[8] = { -1,  0,  1, -1, 1, -1, 0, 1 };

int diContur4Lab5[4] = { -1,  0, 1, 0 };
int djContur4Lab5[4] = { 0, -1, 0, 1 };

void etichetare1()
{
	Mat src;
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		src = imread(fname, IMREAD_GRAYSCALE);

		int vecini;
		printf("Vecinatate 4 sau 8?\n");
		scanf("%d", &vecini);
		if (vecini != 4 && vecini != 8)
		{
			printf("Vecinatatea trebuie sa fie de 4 sau de 8!\n");
			continue;
		}
		//queue<Point> Q;
		int label = 0;

		int height = src.rows;
		int width = src.cols;

		Mat labels(height, width, CV_8UC1);
		Mat binara(height, width, CV_8UC1);
		Mat etichetare(height, width, CV_8UC3);

		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				if (src.at<uchar>(i, j) < 70)
				{
					binara.at<uchar>(i, j) = 0;
				}
				else
				{
					binara.at<uchar>(i, j) = 255;
				}
			}
		}


		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				labels.at<uchar>(i, j) = 0;
			}
		}

		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				if (binara.at<uchar>(i, j) == 0 && labels.at<uchar>(i, j) == 0)
				{
					label++;
					queue<Point> Q;
					labels.at<uchar>(i, j) = label;
					Point p = { i,j };
					Q.push(p);

					while (Q.empty() != true)
					{
						//printf("%d-", label);
						Point p = Q.front(); //asta returneaza
						Q.pop(); //asta il scoate
						if (vecini == 4)
						{
							for (int k = 0; k < 4; k++)
							{
								int iVecin = p.x + diContur4Lab5[k];
								int jVecin = p.y + djContur4Lab5[k];
								if (iVecin < 0 || iVecin > height || jVecin < 0 || jVecin > width)
								{
									continue;
								}
								if (binara.at<uchar>(iVecin, jVecin) == 0 && labels.at<uchar>(iVecin, jVecin) == 0)
								{
									labels.at<uchar>(iVecin, jVecin) = label;
									Point p = { iVecin,jVecin };
									Q.push(p);
									//Q.push({ iVecin,jVecin });
								}
							}
						}
						else
						{
							for (int k = 0; k < 8; k++)
							{
								int iVecin = p.x + diContur8Lab5[k];
								int jVecin = p.y + djContur8Lab5[k];
								if (iVecin < 0 || iVecin > height || jVecin < 0 || jVecin > width)
								{
									continue;
								}
								if (binara.at<uchar>(iVecin, jVecin) == 0 && labels.at<uchar>(iVecin, jVecin) == 0)
								{
									labels.at<uchar>(iVecin, jVecin) = label;
									Q.push({ iVecin,jVecin });
								}
							}
						}
					}
				}
			}
		}

		std::default_random_engine gen;
		std::uniform_int_distribution<int> d(0, 255);

		Vec3b* culori = (Vec3b*)calloc(label + 1, sizeof(Vec3b));
		for (int i = 1; i < label + 1; i++)
		{
			uchar red = d(gen);
			uchar blue = d(gen);
			uchar green = d(gen);
			culori[i] = Vec3b(blue, green, red);
		}
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				for (int z = 1; z <= label; z++)
				{
					if (labels.at<uchar>(i, j) == z)
					{
						etichetare.at<Vec3b>(i, j) = culori[z];
					}
				}
			}
		}
		free(culori);
		imshow("Sursa", src);
		imshow("Labels", labels);
		imshow("Etichetare", etichetare);
		waitKey(0);
	}
}

int minVector(vector<int> lista)
{
	int minim = 99999;
	int size = lista.size();
	for (int i = 1; i < size; i++)
	{
		int valCurenta = lista[i];
		if (valCurenta < minim)
		{
			minim = valCurenta;
		}
	}
	return minim;
}

//etichetare, primeste imaginea, pointer la img etichetata si sirul de culori pe care sa le returneze
int etichetare(Mat src, Mat* etichetare, Vec3b* culori)
{
	int label = 0;

	int height = src.rows;
	int width = src.cols;

	Mat labels(height, width, CV_8UC1);

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			labels.at<uchar>(i, j) = 0;
		}
	}


	vector<vector<int>> edges;

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			if (src.at<uchar>(i, j) == 255 && labels.at<uchar>(i, j) == 0)
			{
				//facem un vector nou
				vector<int> lista;
				lista.push_back(-1);

				for (int k = 0; k < 4; k++)
				{
					int iVecin = i + diAnteriori[k];
					int jVecin = j + djAnteriori[k];
					if (iVecin < 0 || iVecin > height || jVecin < 0 || jVecin > width)
					{
						continue;
					}
					if (labels.at<uchar>(iVecin, jVecin) > 0)
					{
						lista.push_back(labels.at<uchar>(iVecin, jVecin));
					}
				}
				if (lista.size() == 1) //daca e doar acel -1, inseamna ca nu am pus nimic
				{
					//printf("Intra la label++ %d\n",label);
					label++;
					labels.at<uchar>(i, j) = label;
					int newSize = label + 1;
					edges.resize(newSize);

					int oldsize = label - 1;
					edges[oldsize].push_back(-1);
				}
				else
				{
					//printf("Intra pe else\n");
					int minim = minVector(lista);
					labels.at<uchar>(i, j) = minim;

					for (int k = 1; k < lista.size(); k++)
					{
						if (lista[k] != minim)
						{
							edges[minim].push_back(lista[k]);
							edges[lista[k]].push_back(minim);
						}
					}
				}

			}
		}
	}

	int newlabel = 0;
	vector<int> newLabelLista;
	int newSize = label + 1;
	newLabelLista.resize(newSize);

	for (int i = 0; i < newSize; i++)
	{
		newLabelLista[i] = 0;
	}

	for (int i = 1; i < newSize; i++)
	{
		if (newLabelLista[i] == 0)
		{
			newlabel++;
			queue<int> Q;
			newLabelLista[i] = newlabel;
			Q.push(i);

			while (Q.empty() != true)
			{
				int x = Q.front();
				Q.pop();

				int sizeX = edges[x].size();
				for (int j = 0; j < sizeX; j++)
				{
					int y = edges[x][j];
					if (newLabelLista[y] == 0)
					{
						newLabelLista[y] = newlabel;
						Q.push(y);
					}
				}
			}
		}
	}

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			labels.at<uchar>(i, j) = newLabelLista[labels.at<uchar>(i, j)];
		}
	}

	std::default_random_engine gen;
	std::uniform_int_distribution<int> d(0, 255);

	//Vec3b* culori1 = (Vec3b*)calloc(newlabel + 1, sizeof(Vec3b));

	for (int i = 0; i < newlabel; i++)
	{
		uchar red = d(gen);
		uchar blue = d(gen);
		uchar green = d(gen);
		culori[i]=Vec3b(blue, green, red);
	}
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			for (int z = 1; z <= newlabel; z++)
			{
				if (labels.at<uchar>(i, j) == z)
				{
					etichetare->at<Vec3b>(i, j) = culori[z-1];
				}
			}
		}
	}
	//imshow("EtichetataFunctie", *etichetare);
	//waitKey(0);

	return newlabel;
}


int di[8] = { -1,-1,-1,0,0,1,1,1 };
int dj[8] = { -1,0,1,-1,1,-1,0,1 };

int neighbours(int i, int j, Mat src, Vec3b culoare)
{
	int height = src.rows;
	int width = src.cols;
	for (int d = 0; d < 8; d++)
	{
		if (i + di[d] < 0 || i + di[d] > height || j + dj[d] < 0 || j + dj[d] > width)
			continue;
		if (src.at<Vec3b>(i + di[d], j + dj[d]) == culoare)
		{
			return 1;
		}
	}
	return 0;
}

//functia principala care primeste sursa, o proceseaza si afiseaza imaginea finala
Mat detectRed(Mat src)
{
	int height = src.rows;
	int width = src.cols;

	//o matrice grayscale in care cu cat un pixel din imaginea sursa este mai rosu, cu atat pixelul din aceasta imagine este
	//mai alb. Rosu complet va avea valoarea 255 in aceasta imagine.
	//o sa pun un prag in care o sa consider pe care pixeli vreau sa ii procesez in functie de cat de mare ii este intensitatea, adica
	//cat de rosu este
	Mat redness(height, width, CV_8UC1);
	//matricea destinatie in care voi corecta ochii rosii
	Mat dest(height, width, CV_8UC3);
	//inca un mask, o imagine binara in care evidentiez pixelii care sunt cat de cat albi, dupa o formula aleasa de mine.
	//o sa folosesc asta ca sa identific ochii (ochii vor trebui sa aiba zona alba prin jurul lor)
	Mat zoneAlbe(height, width, CV_8UC1);

	int maximInRedness = 0; //calculez maximul ca sa pot scala astfel incat valoarea maxima din redness sa fie 255

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			Vec3b culoare = src.at<Vec3b>(i, j);
			//printf("%d %d %d\n", culoare[0], culoare[1], culoare[2]);
			
			//pentru un pixel anume, calculez care este minimul dintre Green si Blue, si care este maximul
			int minimGB;
			int maximGB;
			if (culoare[0] < culoare[1])
			{
				minimGB = culoare[0];
				maximGB = culoare[1];
			}
			else
			{
				minimGB = culoare[1];
				maximGB = culoare[0];
			}
			
			int valoare;

			if (culoare[2] != 0)
			{
				int howMuchRed = culoare[2] - maximGB;
				//aici, daca pixelul R este mai mic decat max(G,B) consider ca pixelul nu este R, si atunci fac valoarea 0,
				//altfel as putea fi in cazul de exemplu, cand pixelul este aproape albastru(1,0,255). Pentru aceste valori,
				//valoarea finala ar fi (1-255)*(1-255)/255, deci o valoare foarte mare, chiar daca pixelul nu este deloc rosu.
				if (howMuchRed < 0) 

				{
					howMuchRed = 0;
				}

				//asta este o formula pe care am gasit-o pe net:
				//iau valoarea de R din pixel, scad maximul dintre G si B, ridic totul la patrat si dupa imparta la valoarea R
				//asa de exemplu, la un rosu perfect(255,0,0), valoarea va fi 255*255/255=255. Cu cat e mai mare aceasta valoare,
				//atat valoarea va fi mai mare
				valoare = howMuchRed * howMuchRed /culoare[2];
				//valoare = (4 * culoare[2] - (culoare[1] + culoare[0]) - minimGB - maximGB) / culoare[2] * 255 / 4;
			}
			else
			{
				valoare = 0;
			}

			redness.at<uchar>(i, j) = valoare;
			if (valoare > maximInRedness)
			{
				maximInRedness = valoare; //aici calculez valoarea maxima de redness
			}
		}
	}

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			redness.at<uchar>(i, j) = redness.at<uchar>(i, j) * 255 / maximInRedness; //si aici fac scalarea
			//de exemplu, daca ar fi valoarea maxima 240, un pixel cu acea valoare ar deveni 255, un pixel cu valoarea 200, ar fi
			//200 * 255 / 240
		}
	}

	///*
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			Vec3b culoare = src.at<Vec3b>(i, j);

			//aici vreau sa calculez cat de alb este un pixel, ca sa pot calcula maskul de alb
			//calculez media
			int catDeAlb = (culoare[0] + culoare[1] + culoare[2]) / 3;
			if (catDeAlb > 50 && (abs(culoare[0] - culoare[1]) < 20) && (abs(culoare[0] - culoare[2]) < 20) && (abs(culoare[2] - culoare[1]) < 20))
			{
				//si verific daca este cel putin 50, altfel il consider prea negru, si diferentele intre cele 3 canale sa fie mai mici
				//de 20, sa fie cat de cat egale
				zoneAlbe.at<uchar>(i, j) = 255;
			}
		}
	}
	//*/

	//o imagine binara in care daca pixelii din redness au o intensitate minima hardcodata de mine, pun aici alb, altfel pun negru
	//asa o sa procesez doar pixelii care aici sunt albi, pe restul ii ignor
	Mat imgBinara(height, width, CV_8UC1);
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			//am pus threshold-ul 100, ales prin teste
			if (redness.at<uchar>(i, j) > 100)
			{

				imgBinara.at<uchar>(i, j) = 255;
			}
			else
			{
				imgBinara.at<uchar>(i, j) = 0;
			}
		}
	}

	//aici am imaginea etichetata, pornind de la imaginea binara calculata mai sus
	//am facut asta ca sa pot procesa pe rand fiecare obiect rosu din imagine si sa vad daca este ochi si trebuie sa il schimb sau nu
	Mat imgEtichetata(height, width, CV_8UC3);

	//un vector in care voi stoca culorile pentru fiecare obiect in parte. Am pus maxim 256, nu o sa fie asa multe
	Vec3b* culori = (Vec3b*)calloc(256, sizeof(Vec3b));

	//aici apelez functia de etichetare, trimit adresa imaginii, ca sa o pot modifica in functie, si trimit si vectorul de culori
	int sizeCulori = etichetare(imgBinara, &imgEtichetata, culori);

	//o imagine binara dupa ce am procesat imaginea binara anterioara. In primul rand, iau doar obiectele etichetate care au in jurul lor
	//si cateva portiuni albe, ca sa aiba sansa sa fie ochi si in acelasi timp, obiectele etichetate nu sunt deloc asemanatoare cu
	//cercuri, deci calculez o raza maxima si construiesc un nou obiect pornind din centrul obiectului etichetat, si cu acea raza
	Mat binaraMaiBuna(height, width, CV_8UC1);

	//aici voi avea doar obiectele pe care le consider ca sunt ochi rosii, exact pixelii pe care vreau sa ii modific
	Mat imagineDoarCuOchi(height, width, CV_8UC1);

	//aici sunt culorile obiectelor alese
	Vec3b* culoriAlese = (Vec3b*)calloc(sizeCulori, sizeof(Vec3b));
	int contorCuloriAlese = 0;
	//aria si valoarea care reprezinta cat de cerc este acel cerc (teoretic este 1, daca este un cerc perfect)
	int* arie = (int*)calloc(sizeCulori, sizeof(int));
	float* catDeCerc = (float*)calloc(sizeCulori, sizeof(float));

	for (int k = 0; k < sizeCulori; k++)
	{
		int area = 0;
		int xmax = 0;
		int xmin = 10000;
		int ymax = 0;
		int ymin = 10000;

		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				if (imgEtichetata.at<Vec3b>(i, j) == culori[k])
				{
					//pentru fiecare pixel, incrementez aria
					area++;
					//in acelasi timp calculez si care este punctul cel mai de suc, mai de jos, stanga si dreapta
					if (i < ymin)
					{
						ymin = i;
					}
					if (i > ymax)
					{
						ymax = i;
					}
					if (j < xmin)
					{
						xmin = j;
					}
					if (j > xmax)
					{
						xmax = j;
					}
				}
			}
		}

		//Pentru centrul de masa, am luat din lab
		int rcentru = 0;
		int ccentru = 0;

		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				if (imgEtichetata.at<Vec3b>(i, j) == culori[k])
				{
					rcentru = rcentru + i;
					ccentru = ccentru + j;
				}
			}
		}

		rcentru = rcentru / area;
		ccentru = ccentru / area;

		//printf("Rcentru=%d, Ccentru=%d\n", rcentru, ccentru);

		//acum pentru perimetru, am nevoie de perimetru pentru formula care determina cat de cerc este obiectul
		float perimeter = 0;
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				if (imgEtichetata.at<Vec3b>(i, j) == culori[k])
				{
					if (neighbours(i, j, imgEtichetata, culori[k]))
					{
						perimeter++;
					}
				}
			}
		}

		perimeter = perimeter * M_PI / 4;

		//factor de subtiere
		float factorSubtiere = 4 * M_PI * area / (perimeter * perimeter);

		//printf("Culoare: %d %d %d, Cat de cerc este %f, Arie=%d\n",culori[k][2],culori[k][1],culori[k][0], factorSubtiere, area);

		//aici am 4 raze, in sus, in jos, stanga si dreapta pornind din centru. Din cele 4 o aleg pe cea maxima pentru a construi un cerc
		//cu acea raza in loc de ce am detectat eu cu filtrul de rosu
		int razaMaxima = 0;
		int raza1 = abs(rcentru - ymin);
		int raza2 = abs(rcentru - ymax);
		int raza3 = abs(ccentru - xmin);
		int raza4 = abs(ccentru - xmax);
		int aux1;
		int aux2;
		if (raza1 > raza2)
		{
			aux1 = raza1;
		}
		else
		{
			aux1 = raza2;
		}
		if (raza3 > raza4)
		{
			aux2 = raza3;
		}
		else
		{
			aux2 = raza4;
		}
		if (aux1 > aux2)
		{
			razaMaxima = aux1;
		}
		else
		{
			razaMaxima = aux2;
		}

		int howMany = 0;

		//aici verific in cate parti pot gasi zone albe (cum au ochii zone albe in jurul lor) pornind de la centru, de la cel mai de sus punct
		//si de la cel mai de jos punct
		for (int i = 0; i <1.5 * razaMaxima; i++)
		{
			if (zoneAlbe.at<uchar>(rcentru, xmin - i) == 255)
			{
				howMany++;
				break;
			}
		}
		for (int i = 0; i <1.5 * razaMaxima; i++)
		{
			if (zoneAlbe.at<uchar>(rcentru, xmax + i) == 255)
			{
				howMany++;
				break;
			}
		}

		for (int i = 0; i < 2 * razaMaxima; i++)
		{
			if (zoneAlbe.at<uchar>(ymin, xmin - i) == 255)
			{
				howMany++;
				break;
			}
		}
		for (int i = 0; i < 2 * razaMaxima; i++)
		{
			if (zoneAlbe.at<uchar>(ymin, xmax + i) == 255)
			{
				howMany++;
				break;
			}
		}

		for (int i = 0; i < 2 * razaMaxima; i++)
		{
			if (zoneAlbe.at<uchar>(ymax, xmin - i) == 255)
			{
				howMany++;
				break;
			}
		}
		for (int i = 0; i < 2 * razaMaxima; i++)
		{
			if (zoneAlbe.at<uchar>(ymax, xmax + i) == 255)
			{
				howMany++;
				break;
			}
		}

		//daca gasesc cel putin in 2 directii 2 albe, consider, adaug acel obiect intr-o alta imagine intermediara
		if (howMany >= 2)
		{
			for (int i = 0; i < height; i++)
			{
				for (int j = 0; j < width; j++)
				{
					if (sqrt((j - ccentru) * (j - ccentru) + (i - rcentru) * (i - rcentru)) < (razaMaxima + (0.15 * razaMaxima)))
					{
						binaraMaiBuna.at<uchar>(i, j) = 255;
					}

				}
			}
			culoriAlese[contorCuloriAlese] = culori[k];
			arie[contorCuloriAlese] = area;
			catDeCerc[contorCuloriAlese] = factorSubtiere;
			//printf("Culori Alese %d %d %d, Arie %d, Cerc %f\n", culoriAlese[contorCuloriAlese][2], culoriAlese[contorCuloriAlese][1], culoriAlese[contorCuloriAlese][0],arie[contorCuloriAlese],catDeCerc[contorCuloriAlese]);
			contorCuloriAlese++;

		}

		//printf("HowMany:%d\n", howMany);
	}

	Mat binaraFinala(height,width,CV_8UC1);

	//daca am gasit mai putin de 2, le iau pur si simplu fara alte verificari
	if (contorCuloriAlese <= 2)
	{
		for (int k = 0; k < contorCuloriAlese; k++)
		{
			for (int i = 0; i < height; i++)
			{
				for (int j = 0; j < width; j++)
				{
					binaraFinala.at<uchar>(i, j) = binaraMaiBuna.at<uchar>(i, j);
				}
			}
		}
	}
	//daca am gasit mai mult de 2 obiecte rosii care pot fi ochi, verific care 2 sunt cele mai apropiate din punct de vedere al ariei, si al 
	//factorului de subtiere
	else
	{
		printf("ContorCuloriAlese:%d\n", contorCuloriAlese);
		Vec3b culoriAleseFinal[2];
		float diferentaArieMinim = 999999;
		float diferentaCercMinim = 10;

		for (int i = 0; i < contorCuloriAlese; i++)
		{
			printf("Arie %d, Cerc %f\n", arie[i], catDeCerc[i]);
		}

		for (int i = 0; i < contorCuloriAlese - 1; i++)
		{
			for (int j = 0; j < contorCuloriAlese - i - 1; j++)
			{
				int maximArie;
				if (arie[j] < arie[j + 1])
					maximArie = arie[j + 1];
				else
					maximArie = arie[j];

				float maximCerc;
				if (catDeCerc[j] < catDeCerc[j + 1])
					maximCerc = catDeCerc[j + 1];
				else
					maximCerc = catDeCerc[j];

				float raportArie = (float)abs(arie[j] - arie[j + 1]) / maximArie;
				float raportCerc = (float)abs(catDeCerc[j] - catDeCerc[j+1]) / maximCerc;
				if (raportArie < diferentaArieMinim && raportCerc < diferentaCercMinim)
				{
					diferentaArieMinim = raportArie;
					diferentaCercMinim = raportCerc;
					culoriAleseFinal[0] = culoriAlese[j];
					culoriAleseFinal[1] = culoriAlese[j + 1];
				}
			}
		}
		printf("%f %f\n", diferentaArieMinim, diferentaCercMinim);
		printf("CuloriFinala:\n%d %d %d\n%d %d %d\n", culoriAleseFinal[0][2], culoriAleseFinal[0][1], culoriAleseFinal[0][0], culoriAleseFinal[1][2], culoriAleseFinal[1][1], culoriAleseFinal[1][0]);

		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				if (binaraMaiBuna.at<uchar>(i,j)==255 && (imgEtichetata.at<Vec3b>(i,j)==culoriAleseFinal[0] || imgEtichetata.at<Vec3b>(i, j) == culoriAleseFinal[1]))
				{
					binaraFinala.at<uchar>(i, j) = 255;
				}
			}
		}
	}

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			if (binaraFinala.at<uchar>(i, j) == 255)
			{
				//dest.at<Vec3b>(i, j) = Vec3b(src.at<Vec3b>(i, j)[0], src.at<Vec3b>(i, j)[1],(src.at<Vec3b>(i,j)[0]+src.at<Vec3b>(i,j)[1])/2);
				dest.at<Vec3b>(i, j) = Vec3b((src.at<Vec3b>(i, j)[0] + src.at<Vec3b>(i, j)[1]) / 2, (src.at<Vec3b>(i, j)[0] + src.at<Vec3b>(i, j)[1]) / 2, (src.at<Vec3b>(i, j)[0] + src.at<Vec3b>(i, j)[1]) / 2);
				//dest.at<Vec3b>(i, j) = Vec3b(0, 0, 0);
			}
			else
			{
				dest.at<Vec3b>(i, j) = src.at<Vec3b>(i, j);
			}
		}
	}

	imshow("Sursa", src);
	imshow("Redness", redness);
	imshow("BinaraInitial", imgBinara);
	imshow("Etichetata", imgEtichetata);
	//imshow("E", img2);
	//imshow("Destinatie2", binaraMaiBuna);
	imshow("BinaraFinala", binaraFinala);
	imshow("Destinatie1", zoneAlbe);
	imshow("Destinatie", dest);
	free(culori);
	free(arie);
	free(catDeCerc);


	waitKey(0);
}

int main()
{
	char fname[MAX_PATH];

	openFileDlg(fname);
	Mat src = imread(fname,IMREAD_COLOR);
	detectRed(src);
	
	return 0;
}