package service;

import model.FileLoader;
import model.HGVSParser;
import model.Mutation;
import model.ProteinResult;
import model.Transcript;
import model.Translator;

public class MutationService {
	private static void validarLimitesDaMutacao(Transcript transcript,Mutation mutation){
		{
		    int cdsStart = transcript.getCds_start();
		    int cdsEnd = transcript.getCds_end();

		    /*
		     * Caso cdsEnd represente o índice exclusivo do fim da CDS,
		     * o tamanho é calculado por cdsEnd - cdsStart.
		     *
		     * Caso seus metadados utilizem início e fim inclusivos,
		     * use: cdsEnd - cdsStart + 1.
		     */
		    int tamanhoCds = cdsEnd - cdsStart;

		    if (tamanhoCds <= 0) {
		        throw new IllegalStateException(
		                "Os limites da sequência codificante são inválidos.\n"
		                        + "Início da CDS: " + cdsStart + "\n"
		                        + "Fim da CDS: " + cdsEnd
		        );
		    }

		    int posicaoInicial = mutation.getPosition();
		    int tamanhoDelecao = mutation.getDeleteLength();

		    if (posicaoInicial < 1) {
		        throw new IllegalArgumentException(
		                "A posição da mutação deve ser maior ou igual a 1.\n"
		                        + "Posição informada: c." + posicaoInicial
		        );
		    }

		    if (posicaoInicial > tamanhoCds) {
		        throw new IllegalArgumentException(
		                "A posição da mutação está fora da sequência codificante.\n\n"
		                        + "Posição informada: c." + posicaoInicial + "\n"
		                        + "Tamanho da CDS: " + tamanhoCds + " nucleotídeos\n"
		                        + "Intervalo permitido: c.1 até c." + tamanhoCds
		        );
		    }

		    if (tamanhoDelecao < 0) {
		        throw new IllegalArgumentException(
		                "O tamanho da deleção não pode ser negativo.\n"
		                        + "Tamanho informado: " + tamanhoDelecao
		        );
		    }

		    /*
		     * Para deleções, calcula a última base atingida.
		     *
		     * Exemplo:
		     * posição inicial = 10
		     * tamanho da deleção = 3
		     * intervalo afetado = c.10_12
		     */
		    if (tamanhoDelecao > 0) {
		        int posicaoFinal = posicaoInicial + tamanhoDelecao - 1;

		        if (posicaoFinal > tamanhoCds) {
		            int basesDisponiveis = tamanhoCds - posicaoInicial + 1;

		            throw new IllegalArgumentException(
		                    "A deleção informada ultrapassa o final da sequência codificante.\n\n"
		                            + "Intervalo solicitado: c."
		                            + posicaoInicial + "_" + posicaoFinal + "del\n"
		                            + "Tamanho da deleção: "
		                            + tamanhoDelecao + " nucleotídeos\n"
		                            + "Tamanho da CDS: "
		                            + tamanhoCds + " nucleotídeos\n"
		                            + "Bases disponíveis a partir de c."
		                            + posicaoInicial + ": "
		                            + basesDisponiveis + " nucleotídeos\n"
		                            + "Última posição válida: c." + tamanhoCds
		            );
		        }
		    }
		}
	}
	
	
	
	
	
	
	public static ProteinResult processarMutacao(String id, String hgvs) {

	    String[] metadado = FileLoader.findCdsMetadata(id);
	    if (metadado == null) {
	        throw new IllegalArgumentException("Transcrito não encontrado");
	    }

	    String dna = FileLoader.findFastaSequence(id);
	    if (dna == null || dna.isBlank()) {
	        throw new IllegalArgumentException("Sequência não encontrada");
	    }

	    Transcript transcript = new Transcript(
	            metadado[0],
	            dna,
	            metadado[1],
	            Integer.parseInt(metadado[2]),
	            Integer.parseInt(metadado[3])
	    );

	    Mutation mutation = HGVSParser.parse(hgvs);
	    if (mutation == null) {
	        throw new IllegalArgumentException("Mutação HGVS inválida");
	    }

	    validarLimitesDaMutacao(transcript, mutation);

	    Translator translator = new Translator();
	    ProteinResult resultado = translator.process(transcript, mutation);

	    if (resultado == null) {
	        throw new IllegalStateException(
	                "Não foi possível processar a mutação"
	        );
	    }

	    return resultado;
	}
}
