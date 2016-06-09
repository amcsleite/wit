#include "Essential.h"
#include "Generator.h"
#include "DIGenerator.h"
#include "Expression.h"
#include "ExpressionList.h"
#include "Term.h"
#include "Util.h"

extern "C"
{
    int mainLib( int argc, const char* argv[] );
}

int runTests (int argc, char* argv[]);

void printUsageMsg()
{
    printf("\n--------------------------------------------------------\n");
    printf("Witness generator usage:\n");
    printf("  wit P M b <enter> \n");
    printf("  P number of preparations\n");
    printf("  M number of measurements\n");
    printf("  d dimension of the classical channel same as number of classic states \n");
    printf("  b number of outputs \n");
    printf("\n--------------------------------------------------------\n");
}

int main (int argc, char* argv[])
{
    if (argc == 2)
    {
        if(!Util::mkFolder(0, 0, 0, 0))
        {
            Util::errorMsg("couldn't create device folder");
            exit(EXIT_FAILURE);
        }

        if(chdir(Util::getFolderName()) != 0)
        {
            Util::errorMsg("couldn't cd to device folder");
            exit(EXIT_FAILURE);
        }

        runTests(argc, argv);
        //system("emacs log.log");

        exit(EXIT_SUCCESS);
    }

    if (argc != 5)
    {
        Util::errorMsg("This program takes exactly 4 arguments.");
        printUsageMsg();
        exit(EXIT_FAILURE);
    }

    int preparations = atol(argv[1]);
    int measurements = atol(argv[2]);
    int channelDimensions = atol(argv[3]);
    int outputs = atol(argv[4]);

    // args sanity check

    if (preparations <= 0 || preparations > 100)
    {
        Util::errorMsg("Invalid number of preparations");
        exit(EXIT_FAILURE);
    }

    if (measurements <= 0 || measurements > 100)
    {
        Util::errorMsg("Invalid number of measurements");
        exit(EXIT_FAILURE);
    }

    if (channelDimensions < 2 || channelDimensions > 100)
    {
        Util::errorMsg("Invalid number of dimensions. (dimension must be >= 2) ");
        exit(EXIT_FAILURE);
    }

    if (outputs < 2 || outputs > 100)
    {
        Util::errorMsg("Invalid number of outputs. (must be >= 2) ");
        exit(EXIT_FAILURE);
    }

    if(!Util::mkFolder(preparations, measurements, channelDimensions, outputs))
    {
        Util::errorMsg("couldn't create device folder");
        exit(EXIT_FAILURE);
    }

    if(chdir(Util::getFolderName()) != 0)
    {
        Util::errorMsg("couldn't cd to device folder");
        exit(EXIT_FAILURE);
    }

	unlink("complementary.poi");
	unlink("log.log");
	unlink("porta.log");
	unlink("porta.poi");
	unlink("lrs.hf");
	unlink("porta.poi.ieq");
	unlink("porta.stdout");

	time_t t = time(NULL);
	ilog("This run is starting at: %s ", ctime(&t));

	ilog("- Models classic devices with:");
	ilog("%d preparations, %d measurements %d dimensions %d outputs -", preparations, measurements, channelDimensions, outputs);
	
	ilog("");
    GeneratorPtr pBase = new Generator();
    ExpressionListPtr pS = new ExpressionList();

    ExpressionListPtr pStrategyList = new ExpressionList();

    DIGeneratorPtr pG = new DIGenerator(preparations, measurements, channelDimensions, outputs);

    printf("\n-----------------------------------------------------------------------------");
    printf("\n Results and intermediate files are in the folder: %s", Util::getFolderName());
    printf("\n-----------------------------------------------------------------------------");

    printf("\nPolytope prepared for porta with d = %d  (c.f. porta.poi)", pG->getDimensionsTotal());

     // generate the file from device

    pG->generate(pStrategyList);

	// ExpressionListPtr pGrayList = ExpressionList::gray(pG->getDimensionsTotal());

    //ExpressionListPtr pSelected = pGrayList->select(pStrategyList);

    pStrategyList->writePorta("porta.poi", outputs, pG->getDimensionsTotal());

    pStrategyList->writeLRS("lrs.hf", outputs, pG->getDimensionsTotal(), preparations, measurements, channelDimensions, outputs);
    //pStrategyList->printResult(stdout);


    // generate the completary file

    // generate the file from device

	log("\n------------- generated in base --------------------------------------------------------------------------------------\n");
	pStrategyList->print();

	// close log file as we going to fork process otherwise there is the risk of a race condition
	// next log will open for append
	
	LogFile::staticClose();

    // prepare to run porta in a separated process
    pid_t pid = fork();

    if (pid == 0)
    {
		// run porta ins aseparate process and redirect its output to porta.stdout
        int oF = open("porta.stdout",  O_WRONLY | O_CREAT, S_IRWXU);

        if (oF == -1)
        {
            XK(false);
            exit(EXIT_FAILURE);
        }

        dup2(oF, STDOUT_FILENO);

        const char* args[] = {"porta", "-T", "porta.poi"};
        mainLib(3, args);

        close(oF);
    }
    else
    {
        int status;
        printf("\n running porta ...\n\n");
        printf("\n you may use:  tail -f porta.stdout to see how porta is progressing\n");
        fflush(stdout);
        waitpid(pid, &status, WUNTRACED);
    }

    printf("\n porta has finished" );
    printf("\n filtering the inequations ...");

    //ExpressionListPtr pPorta, pSorted, pCompacted, pCompactedInverses;

	log("\n----------------------------------------------------------------------------------------------------------------------\n");

    //filterPorta(preparations, measurements, pPorta, pSorted, pCompacted, pCompactedInverses);

    //log("\n\n  porta vertex verification\n");
    //bool b = pPorta->verify(pStrategyList);
    //log("\n\n  porta vertex verification %s - \n\n", b ? "suceeded" : "failed");
	//log("\n-----------------------------------------------------------------------------------------------------------------------\n");

	// verification must rotate each witness !!
    /*log("\n\n sorted vertex verification\n");
    b = pSorted->verify(pSelected);
    log("\n\n - sorted vertex verification %s - \n\n", b ? "suceeded" : "failed");
	log("\n-----------------------------------------------------------------------------------------------------------------------\n");
	*/
	/*
    log("\n\n compacted vertex verification\n");
    b = pCompacted->verify(pSelected);
    log("\n\n - compacted vertex verification %s - \n\n", b ? "suceeded" : "failed");
	log("\n-----------------------------------------------------------------------------------------------------------------------\n");
	*/
	
    /*log("\n\n inverse compact vertex verification\n");
    b = pCompactedInverses->verify(pSelected);
    log("\n\n - inverse compact vertex verification %s - \n\n", b ? "suceeded" : "failed");
	log("\n-----------------------------------------------------------------------------------------------------------------------\n");
	*/

	time_t t2 = time(NULL);
	
	ilog("started at %s finished at: %s (took ~%d hours) ", ctime(&t), ctime(&t2), (t2 - t) / 3600); 
    ilog("\n\n");
	LogFile::staticClose();
    system("emacs log.log");
    printf("\n\n");
    exit(EXIT_SUCCESS);
}

// eof
