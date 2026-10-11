// Writes every function Ghidra found to a CSV: address, size, ranges, convention, name.
// Size is the extent of the function's body; ranges counts its separate address blocks.
// @category ra2-decomp
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.AddressRange;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionIterator;
import java.io.PrintWriter;

public class ExportFunctions extends GhidraScript {
    @Override
    protected void run() throws Exception {
        String out = getScriptArgs()[0];
        try (PrintWriter w = new PrintWriter(out)) {
            w.println("address,size,ranges,thunk,convention,name");
            FunctionIterator it = currentProgram.getFunctionManager().getFunctions(true);
            while (it.hasNext()) {
                Function f = it.next();
                long size = 0;
                int ranges = 0;
                for (AddressRange r : f.getBody()) {
                    size += r.getLength();
                    ranges++;
                }
                w.printf("0x%08x,%d,%d,%d,%s,%s%n", f.getEntryPoint().getOffset(), size, ranges,
                    f.isThunk() ? 1 : 0, f.getCallingConventionName(),
                    f.getName(true).replace(',', ';'));
            }
        }
    }
}
