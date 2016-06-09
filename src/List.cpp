#include <iostream>
#include <armadillo>
#include "Util.h"

LogStream g_logStream("wList.log");

LogStream g_out("list.csv", false);
LogStream g_html("list.html", false);

N32 preparations, measurements, states, outcomes;

bool _gHeader = false;

void listHtml(ExpressionList* pL, Expression* p)
{
	N32 POVMStatus;
	N32 RealPOVMStatus;
	
	N32 ProjectiveStatus;
	N32 RealProjectiveStatus;
	
    double SDPValue;
    double RealPOVMValue;
    double POVMValue;
    double ProjectiveValue;
    double RealProjectiveValue;


    N32 SDPStatus;

	N32 isProjective;
	
	p->readInt("ProjectiveStatus", ProjectiveStatus, 0.);
    p->readInt("RealProjectiveStatus", RealProjectiveStatus, 0.);
	
    p->readDouble("ProjectiveValue", ProjectiveValue, 0.);
    p->readDouble("RealProjectiveValue", RealProjectiveValue, 0.);

	p->readInt("POVMStatus", POVMStatus, 0.);
    p->readInt("RealPOVMStatus", RealPOVMStatus, 0.);
    p->readDouble("POVMValue", POVMValue, 0.);
    p->readDouble("RealPOVMValue", RealPOVMValue, 0.);


    p->readInt("ProjectiveStatus", ProjectiveStatus, 0);
    p->readInt("POVMStatus", POVMStatus, 0);

	N32 MProjStatus, SSStatus, SSRealStatus;
    p->readInt("MProjStatus", MProjStatus, 0);
    p->readInt("SSStatus", SSStatus, 0);
    p->readInt("SSRealStatus", SSRealStatus, 0);
	double MProjValue, SSValue, SSRealValue;
	
    p->readDouble("MProjValue", MProjValue, 0.);
    p->readDouble("SSValue", SSValue, 0.);
    p->readDouble("SSRealValue", SSRealValue, 0.);

    N32 pos = 0;
	char szAll[2048];
    
	pos += sprintf(szAll + pos, "<tr>" );
	pos += sprintf(szAll + pos, "</tr>" );

    pos += sprintf(szAll + pos, "<td>%d</td>", p->getID());
    pos += sprintf(szAll + pos, "<td>%d</td>", p->getRHS());

    pos += sprintf(szAll + pos, "<td>%f</td>", ProjectiveValue);
    pos += sprintf(szAll + pos, "<td>%f</td>", RealProjectiveValue);
    pos += sprintf(szAll + pos, "<td>%f</td>", POVMValue);
    pos += sprintf(szAll + pos, "<td>%f</td>", RealPOVMValue);

    pos += sprintf(szAll + pos, "<td>%f</td>", MProjValue);
    pos += sprintf(szAll + pos, "<td>%f</td>", SSValue);
    pos += sprintf(szAll + pos, "<td>%f</td>", SSRealValue);

	pos += sprintf(szAll + pos, "<td>%d</td>", ProjectiveStatus);
	pos += sprintf(szAll + pos, "<td>%d</td>", RealProjectiveStatus);
	pos += sprintf(szAll + pos, "<td>%d</td>", POVMStatus);
	pos += sprintf(szAll + pos, "<td>%d</td>", RealPOVMStatus);

	pos += sprintf(szAll + pos, "<td>%d</td>", MProjStatus);
	pos += sprintf(szAll + pos, "<td>%d</td>", SSStatus);
	pos += sprintf(szAll + pos, "<td>%d</td>", SSRealStatus);

	
	pos += sprintf(szAll + pos, "<td><a href=\"http://terminator/ac/%d-%d-%d-%d/wit/%d/wit.pdf\">pdf</a></td>", preparations, measurements, states, outcomes, p->getID());


	/*
    pos += sprintf(szAll + pos, "%d, %d, ", p->getID(), p->getRHS());
           
    pos += sprintf(szAll + pos, "%10.6f, %10.6f, %10.6f, %10.6f, %10.6f, ", SDPValue, ProjectiveValue, RealProjectiveValue, POVMValue, RealPOVMValue);

    pos += sprintf(szAll + pos, "%d, %d, %d, %d, %d, ", SDPStatus, ProjectiveStatus, RealProjectiveStatus, POVMStatus, RealPOVMStatus);
           
	pos += sprintf(szAll + pos, "%d, %d, %d, %d, %s ", preparations, measurements, states, outcomes, witness);*/

	g_html.ws(szAll);           
}

void list(ExpressionList* pL, Expression* p)
{
	N32 POVMStatus;
	N32 RealPOVMStatus;
	
	N32 ProjectiveStatus;
	N32 RealProjectiveStatus;
	
    double SDPValue;
    double RealPOVMValue;
    double POVMValue;
    double ProjectiveValue;
    double RealProjectiveValue;

    N32 SDPStatus;

	N32 isProjective;
	
    p->readDouble("SDPValue", SDPValue, 0.);

	p->readInt("ProjectiveStatus", ProjectiveStatus, 0.);
    p->readInt("RealProjectiveStatus", RealProjectiveStatus, 0.);
	
    p->readDouble("ProjectiveValue", ProjectiveValue, 0.);
    p->readDouble("RealProjectiveValue", RealProjectiveValue, 0.);

	p->readInt("POVMStatus", POVMStatus, 0.);
    p->readInt("RealPOVMStatus", RealPOVMStatus, 0.);
    p->readDouble("POVMValue", POVMValue, 0.);
    p->readDouble("RealPOVMValue", RealPOVMValue, 0.);

    p->readInt("ProjectiveStatus", ProjectiveStatus, 0);
    p->readInt("POVMStatus", POVMStatus, 0);
    p->readInt("SDPStatus", SDPStatus, 0);

    char witness[2048];
    N32 pos = 0;
    witness[pos] = 0;

    N32 w = 0;
    w = sprintf(witness + pos, ",,");
    XK(w > 0);
    pos += w;

    for (N32 r = 0; r < p->getRows(); ++r)
    {
        for (N32 c = 0; c < p->getCols(); ++c)
        {
            for (N32 v = 0; v < p->getVs(); ++v)
            {
                w = sprintf(witness + pos, "%d,", p->get(r, c, v));
                XK(w > 0);
                pos += w;
            }

            w = sprintf(witness + pos, ",");
            XK(w > 0);
            pos += w;
        }

        w = sprintf(witness + pos, ",");
        XK(w > 0);
        pos += w;
    }

	char szAll[2048];
	pos = 0;
	
    if (!_gHeader)
    {
        _gHeader = true;
        pos += sprintf(szAll + pos, "Id, C, " );
        pos += sprintf(szAll + pos, "SDPValue, ProjectiveValue, RealProjectiveValue, POVMValue, RealPOVMValue, ");
        pos += sprintf(szAll + pos, "MProjValue, SSValue, SSRealValue, ");
        pos += sprintf(szAll + pos, "SDPStatus, ProjectiveStatus, RealProjectiveStatus, POVMStatus, RealPOVMStatus, MProjStatus, SSStatus, SSRealStatus" );
        pos += sprintf(szAll + pos, "preparations, measurements, states, outcomes, " );
		g_out.ws(szAll);           
		pos = 0;
    }

    pos += sprintf(szAll + pos, "%d, %d, ", p->getID(), p->getRHS());
           
    pos += sprintf(szAll + pos, "%10.6f, %10.6f, %10.6f, %10.6f, %10.6f, ", SDPValue, ProjectiveValue, RealProjectiveValue, POVMValue, RealPOVMValue);

    pos += sprintf(szAll + pos, "%d, %d, %d, %d, %d, ", SDPStatus, ProjectiveStatus, RealProjectiveStatus, POVMStatus, RealPOVMStatus);
           
	pos += sprintf(szAll + pos, "%d, %d, %d, %d, %s ", preparations, measurements, states, outcomes, witness);

	g_out.ws(szAll);           
}

char szHtmlHeader[] = "\
<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \
    \"http://www.w3.org/TR/html4/strict.dtd\"> \
<html lang=\"en\">\
  <head>\
    <meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\">\
    <title>title</title>\
  </head>\
  <body>\
";

char szHtmlFooter[] = "</body></html>";

int main(int argc, char** argv)
{
    bool fProjective = true;
    bool fPOVM = true;

    strcpy(Util::s_folderName, ".");

    for (N32 n = 1; n < argc; ++n)
    {
        if (argv[n][0] == 'c')
        {
            g_logStream.setUseCout();
        }

        if (argv[n][0] == 'z')
        {
            // do Gallego witness
            if(chdir("/home/al/wTools/witnesses/3-2-2-2") != 0)
                exit(EXIT_FAILURE);
        }
    }

    if (!Util::readIntParam("db/preparations", preparations, 3))
    {
        Util::errorMsg("preparations parameter file not found");
        exit(EXIT_FAILURE);
    }

    if (!Util::readIntParam("db/measurements", measurements, 2))
    {
        Util::errorMsg(" measurements file not found");
        exit(EXIT_FAILURE);
    }

    if(!Util::readIntParam("db/states", states, 2))
    {
        Util::errorMsg("states parameter file not found");
        exit(EXIT_FAILURE);
    }

    if(!Util::readIntParam("db/outcomes", outcomes, 2))
    {
        Util::errorMsg("outcomes parameter file not found");
        exit(EXIT_FAILURE);
    }

    N32 maxIterations = 2000;

    if(!Util::readIntParam("db/maxIterations", maxIterations, 2000))
    {
        Util::errorMsg("maximum iterations parameter not set so I will be using the default: 2000");
    }

    N32 vars = outcomes - 1;

    ExpressionListPtr pList = new ExpressionList;
    Util::parsePortaIEQ("selected.ieq", pList, preparations, measurements, vars);

    pList->readListVars();

    for (Expression* p = pList->head(); p; p = p->next())
    {
        list(pList,  p);
    }

	g_html.ws(szHtmlHeader);


    {
		char szAll[2048];
		N32 pos = 0;
		pos += sprintf(szAll + pos, "<table border=\"1\"  cellpadding =\"5\" style=\"border-collapse:collapse;\">");
        pos += sprintf(szAll + pos, "<tr>" );
        pos += sprintf(szAll + pos, "<td>Id</td>" );
        pos += sprintf(szAll + pos, "<td>C</td>" );
        pos += sprintf(szAll + pos, "<td>ProjectiveValue</td>" );
        pos += sprintf(szAll + pos, "<td>RealProjectiveValue</td>" );
        pos += sprintf(szAll + pos, "<td>POVMValue</td>" );
        pos += sprintf(szAll + pos, "<td>RealPOVMValue</td>" );
		
        pos += sprintf(szAll + pos, "<td>MProjValue</td>" );
        pos += sprintf(szAll + pos, "<td>SSValue</td>" );
        pos += sprintf(szAll + pos, "<td>SSRealValue</td>" );

        pos += sprintf(szAll + pos, "<td>ProjectiveStatus</td>" );
        pos += sprintf(szAll + pos, "<td>RealProjectiveStatus</td>" );
        pos += sprintf(szAll + pos, "<td>POVMStatus</td>" );
        pos += sprintf(szAll + pos, "<td>RealPOVMStatus</td>" );

        pos += sprintf(szAll + pos, "<td>MProjStatus</td>" );
        pos += sprintf(szAll + pos, "<td>SSStatus</td>" );
        pos += sprintf(szAll + pos, "<td>SSRealStatus</td>" );

        pos += sprintf(szAll + pos, "<td>table</td>" );
        pos += sprintf(szAll + pos, "</tr>" );
		
        /*pos += sprintf(szAll + pos, "Id, C, " );
        pos += sprintf(szAll + pos, "SDPValue, ProjectiveValue, RealProjectiveValue, POVMValue, RealPOVMValue, ");
        pos += sprintf(szAll + pos, "SDPStatus, ProjectiveStatus, RealProjectiveStatus, POVMStatus, RealPOVMStatus, " );
        pos += sprintf(szAll + pos, "preparations, measurements, states, outcomes, " );
        pos += sprintf(szAll + pos, "</tr>" );*/
		g_html.ws(szAll);           
		pos = 0;
    }

	for (Expression* p = pList->head(); p; p = p->next())
    {
        listHtml(pList,  p);
    }

	g_html.ws("</table>");
	g_html.ws(szHtmlFooter);

    return 0;
}

//eof
