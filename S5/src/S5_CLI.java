import java.io.*;
import java.util.*;
import ttp.Optimisation.Optimisation;
import ttp.TTPInstance;
import ttp.TTPSolution;
import ttp.Utils.Utils;
import ttp.Utils.drawTSP;

public class S5_CLI {
    public static void main(String[] args) {
        // TTP instance path
        final String inst = args[0];

        // output file
        final String outputFile;
        if (args.length >= 2)
            outputFile = args[1];
        else
            outputFile = null;

        // log file
        final String logFile;
        if (outputFile != null){
            logFile = outputFile + ".log";
        }else{
            logFile = null;
        }
        // runtime limit
        int runtimeLimit = 10;
        if (args.length >= 3)
            runtimeLimit = Integer.parseInt(args[2]);

        // seed
        int seed = -1;
        if (args.length >=4) {
            seed = Integer.parseInt(args[3]);
        } else {
            seed = (int)(Math.random()*Integer.MAX_VALUE);
        }

        Utils.setRandomNumberSeed(seed);
        Optimisation.setRandomNumberSeed(seed);

        int maxRuntime = runtimeLimit * 1000;

        File f = new File(inst);
        TTPInstance instance = new TTPInstance(f);

        int[] tour= null;
        long startTime = System.currentTimeMillis();
        System.out.print(f.getName()+": ");

        TTPSolution newSolution = new TTPSolution(Optimisation.linkernTour(instance.file.getPath(), instance.numberOfNodes+1), new int[instance.numberOfItems]);
        instance.evaluate(newSolution, false);

        ArrayList<Pair<TTPSolution, Long>> logs = new ArrayList<>();

        long starttime = System.currentTimeMillis();
        long currenttime = starttime;

        long minutes_passed = 1;
        long log_interval = 60 * 1000;

        for(long lastDuration = 0L; currenttime < (startTime + maxRuntime) - lastDuration * 2D; lastDuration = currenttime - starttime)
        {
            starttime = System.currentTimeMillis();
            TTPSolution tempSolution = Optimisation.HT(instance, maxRuntime, true);
            if(newSolution == null || tempSolution.objectiveScore > newSolution.objectiveScore)
                newSolution = tempSolution;
            currenttime = System.currentTimeMillis();
            if ((currenttime - startTime) / log_interval >= minutes_passed){
                logs.add(new Pair<>(newSolution, (currenttime - startTime)));
                minutes_passed += (currenttime - startTime) / log_interval - minutes_passed + 1;
            }
        }
        currenttime = System.currentTimeMillis();
        logs.add(new Pair<>(newSolution, (currenttime - startTime)));

        if (outputFile != null)
            newSolution.writeResult(outputFile);

        if (logFile != null){
            BufferedWriter writer = null;
            try {
                writer = new BufferedWriter(new FileWriter(logFile, false));
                for (Pair<TTPSolution, Long> log : logs) {
                    writer.write(log.y + "\r\n");
                    writer.write(log.x.answer());
                }
                writer.flush();
                writer.close();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

        newSolution.printFullForCCode();
    }
}
