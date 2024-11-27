#include <stdbool.h>
#include "./libs/lib.h"

#define TAILLE_PAGE 1024

unsigned int calculerNumeroDePage(unsigned long adresse)
{
	return adresse / TAILLE_PAGE;
}

unsigned long calculerDeplacementDansLaPage(unsigned long adresse)
{
	return adresse % TAILLE_PAGE;
}

unsigned long calculerAdresseComplete(unsigned int numeroDePage, unsigned long deplacementDansLaPage)
{
	return numeroDePage * TAILLE_PAGE + deplacementDansLaPage;
}

void rechercherTLB(struct RequeteMemoire *req, struct SystemeMemoire *mem)
{
	for (int i = 0; i < TAILLE_TLB; i++)
	{
		if (mem->tlb->numeroPage[i] == calculerNumeroDePage(req->adresseVirtuelle) && mem->tlb->entreeValide[i])
		{

			req->adressePhysique = calculerAdresseComplete(mem->tlb->numeroCadre[i], calculerDeplacementDansLaPage(req->adresseVirtuelle));
			mem->tlb->dernierAcces[i] = req->date;
			req->estDansTLB = 1;

			return;
		}
	}
	req->adressePhysique = 0;
	req->estDansTLB = 0;
}

void rechercherTableDesPages(struct RequeteMemoire *req, struct SystemeMemoire *mem)
{
	int numPageReq = calculerNumeroDePage(req->adresseVirtuelle);
	if (mem->tp->entreeValide[numPageReq])
	{

		req->adressePhysique = calculerAdresseComplete(mem->tp->numeroCadre[numPageReq], calculerDeplacementDansLaPage(req->adresseVirtuelle));
		req->estDansTablePages = 1;

		return;
	}
	req->adressePhysique = 0;
	req->estDansTablePages = 0;
}

void ajouterDansMemoire(struct RequeteMemoire *req, struct SystemeMemoire *mem)
{
	int numPageReq = calculerNumeroDePage(req->adresseVirtuelle);

	for (int i = 0; i < TAILLE_MEMOIRE; i++)
	{
		if (mem->memoire->utilisee[i] == 0)
		{

			mem->memoire->numeroPage[i] = numPageReq;
			mem->memoire->dateCreation[i] = req->date;
			mem->memoire->dernierAcces[i] = req->date;
			mem->memoire->utilisee[i] = 1;

			// il faut update la table des pages aussi
			mem->tp->numeroCadre[numPageReq] = i; // i est num du cadre libre
			mem->tp->entreeValide[numPageReq] = 1;

			req->adressePhysique = calculerAdresseComplete(i, calculerDeplacementDansLaPage(req->adresseVirtuelle));
			req->estDansTablePages = 1;

			return;
		}
	}
}

void mettreAJourTLB(struct RequeteMemoire *req, struct SystemeMemoire *mem)
{
	int indexToReplace = 0;

	for (int i = 0; i < TAILLE_TLB; i++)
	{ // une entree invalide est prio, ne marche pas sans cette verif
		if (mem->tlb->entreeValide[i] == 0)
		{
			indexToReplace = i;
			break;
		}

		if (mem->tlb->dateCreation[i] < mem->tlb->dateCreation[indexToReplace])
		{
			indexToReplace = i;
		}
	}
	mem->tlb->numeroPage[indexToReplace] = calculerNumeroDePage(req->adresseVirtuelle);
	mem->tlb->numeroCadre[indexToReplace] = calculerNumeroDePage(req->adressePhysique);
	mem->tlb->entreeValide[indexToReplace] = 1;
	mem->tlb->dernierAcces[indexToReplace] = req->date;
	mem->tlb->dateCreation[indexToReplace] = req->date;

	return;
}

// NE PAS MODIFIER
int main()
{
	evaluate(
		&calculerNumeroDePage,
		&calculerDeplacementDansLaPage,
		&calculerAdresseComplete,
		&rechercherTLB,
		&rechercherTableDesPages,
		&mettreAJourTLB,
		&ajouterDansMemoire);
	return 0;
}