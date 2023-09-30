import mars.*;
import java.util.*;

public class MarsCompiler
{
  public static void main(String... args) throws Exception
  {
      if (args.length != 1)
      {
          System.err.println("Usage: java MarsCompiler input");
          System.exit(1);
      }


      Globals.initialize(false);

      MIPSprogram program = new MIPSprogram();
      program.readSource(args[0]);

      ErrorList errors = null;

      try
      {
        program.tokenize();
        errors = program.assemble(new ArrayList(Arrays.asList(program)), true, true);
      }
      catch (ProcessingException e)
      {
        errors = e.errors();
      }

      if (errors.errorsOccurred() || errors.warningsOccurred())
      {
          for (ErrorMessage em : (ArrayList<ErrorMessage>)errors.getErrorMessages())
          {
              System.err.println(String.format("[%s] %s@%d:%d %s",
                em.isWarning() ? "WRN" : "ERR",
                em.getFilename(), em.getLine(), em.getPosition(),
                em.getMessage()));
          }
          System.exit(2);
      }

      for (ProgramStatement ps : (ArrayList<ProgramStatement>)program.getMachineList())
        System.out.println(String.format("%08x %08x", ps.getAddress(), ps.getBinaryStatement()));
      
  }

}