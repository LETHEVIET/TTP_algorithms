package mantesting;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.concurrent.ForkJoinPool;

public class RunExperiment {
    public static void main(String[] args) {
//        String[] instance_list = {
//                "eil76_n75_bounded-strongly-corr_01.ttp",
//                "kroA100_n99_bounded-strongly-corr_01.ttp",
//                "ch130_n129_bounded-strongly-corr_01.ttp",
//                "u159_n158_bounded-strongly-corr_01.ttp",
//                "a280_n279_bounded-strongly-corr_01.ttp"
//        };

//        for (String s : instance_list) {
//            System.out.println(s);
//            String[] arg = {s, "cs2sa"};
//            CS2SA_CLI.main(arg);
//        }

        System.out.println(Runtime.getRuntime().availableProcessors());

        List<String> listOfInstances = Arrays.asList(
                "eil76_n75_bounded-strongly-corr_01.ttp",
                "kroA100_n99_bounded-strongly-corr_01.ttp",
                "ch130_n129_bounded-strongly-corr_01.ttp",
                "u159_n158_bounded-strongly-corr_01.ttp",
                "a280_n279_bounded-strongly-corr_01.ttp"
        );

        ForkJoinPool customThreadPool = new ForkJoinPool(3);
        customThreadPool.submit(() ->
                listOfInstances.parallelStream().forEach(instance -> {
                        System.out.println(instance);
                        String[] arg = {instance, "cs2sa"};
                        CS2SA_CLI.main(arg);
                    }
                )
        );
    }
}
