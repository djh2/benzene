//----------------------------------------------------------------------------
/** @file MoHexMain.cpp */
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include "config.h"
#include "Misc.hpp"
#include "MoHexEngine.hpp"
#include "MoHexPlayer.hpp"
#include "MoHexProgram.hpp"

using namespace benzene;

//----------------------------------------------------------------------------

/** @page mohexmainpage MoHex

    @section overview Overview

    MoHex is Hex player that uses monte-carlo tree search with
    knowledge computation. It links with the UCT search classes from
    the Fuego library.
    
    MoHex uses HexBoard to compute VCs/fillin for nodes in the search
    tree that have been visited more than a certain threshold number
    of times.

    @section classes Classes
    - MoHexEngine
    - MoHexPlayer
    - MoHexSearch

    @section htpcommands HTP Commands
    - @ref hexhtpenginecommands
    - @ref benzenehtpenginecommands

    @todo Add more documentation about MoHex!
*/

//----------------------------------------------------------------------------

namespace {

const char* build_date = __DATE__;

}

//----------------------------------------------------------------------------
void ParseJoblevelCmd(std::string strmoves, std::string strcmds, std::vector<std::string> &cmds)
{
  //parse gtp cmds from argc and argv and save to vector.
  static bool isBlack = true;
  struct MoveToCmd
  {
    std::string operator()(std::string const &str) { 
      std::string ret = isBlack?("play b"+str):("play w"+str); isBlack != isBlack; 
      return ret; 
    }
  };
  struct ExtractCmd
  {
    std::string operator()(std::string const &str) { 
      return str; 
    }
  };

  boost::char_separator<char> sep(",");  

  
  boost::tokenizer<boost::char_separator<char> > tok(strmoves,sep);  
  boost::tokenizer<boost::char_separator<char> > cmdtok(strcmds,sep);

  for(boost::tokenizer< boost::char_separator<char> >::iterator beg=tok.begin(); beg!=tok.end();++beg){
    std::string str = std::string(*beg);
    std::string ret = isBlack?("play b "+str):("play w "+str); 
    isBlack = !isBlack;
    cmds.push_back(ret);
  }
  for(boost::tokenizer< boost::char_separator<char> >::iterator beg=cmdtok.begin(); beg!=cmdtok.end();++beg){
    std::string ret = std::string(*beg);
    cmds.push_back(ret);
  }
  if(cmds.size() > 0)
    cmds.push_back(std::string("quit"));
}

//----------------------------------------------------------------------------
void JobLevelCommander(std::vector<std::string> cmds)
{
  //fork then wait, and write to stdin.
  int pipefd[2]={0};
  if(pipe(pipefd) < 0)
  {
    std::cout<<"fail to create pipe"<<std::endl;
    return;
  }

  int pid = fork();
  if(pid == 0)
  {
    sleep(2);
    close(pipefd[0]);
    dup2(pipefd[1],fileno(stdout));  //output to pipe
    for(int i = 0; i < cmds.size(); i++)
      {
	std::cout<<cmds[i].c_str()<<std::endl; 
      }
  }
  else
  {
    close(pipefd[1]);
    dup2(pipefd[0],fileno(stdin));  //intput from pipe
  }
}


//----------------------------------------------------------------------------
int main(int argc, char** argv)
{
    MiscUtil::FindProgramDir(argc, argv);
    MoHexProgram program(VERSION, build_date);
    BenzeneEnvironment::Get().RegisterProgram(program);
    program.Initialize(argc, argv);
    MoHexPlayer player;
    std::vector<std::string> jobLevelCmds;

    try
    {
        MoHexEngine gh(program.BoardSize(), player);
            std::string config = program.ConfigFileToExecute();
        if (config != "")
            gh.ExecuteFile(config);

	//------------------------------------------------------------------
	ParseJoblevelCmd(program.GetMoves(),program.GetCmds(),jobLevelCmds);
	if(jobLevelCmds.size() > 0)
	{
	  JobLevelCommander(jobLevelCmds);
	}
	//------------------------------------------------------------------

        GtpInputStream gin(std::cin);
        GtpOutputStream gout(std::cout);
        gh.MainLoop(gin, gout);
        program.Shutdown();
    }
    catch (const GtpFailure& f)
    {
        std::cerr << f.Response() << std::endl;
        return 1;
    }
    return 0;
}

//----------------------------------------------------------------------------
