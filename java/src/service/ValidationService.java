package service;
import java.util.regex.Pattern;

/*Classe responsável pela validação de entrada do usuário*/	
	public class ValidationService {

	    private static final String REGEX_TRANSCRIPT = "^NM_\\d+(\\.\\d+)?$";
	    private static final String REGEX_HGVS = "^c\\..+";

	    private static final Pattern PATTERN_TRANSCRIPT = Pattern.compile(REGEX_TRANSCRIPT);
	    private static final Pattern PATTERN_HGVS = Pattern.compile(REGEX_HGVS);

	    public static boolean validateTranscript(String transcript) {
	        return PATTERN_TRANSCRIPT.matcher(transcript).matches();
	    }

	    public static boolean validateHGVS(String hgvs) {
	        return PATTERN_HGVS.matcher(hgvs).matches();
	    }
}

