package service;

import model.FileLoader;
import model.HGVSParser;
import model.Mutation;
import model.ProteinResult;
import model.Transcript;
import model.Translator;

public class MutationService {

    public static ProteinResult processarMutacao(String id, String hgvs) {

        String[] metadado = FileLoader.findCdsMetadata(id);
        if (metadado == null) {
            throw new IllegalArgumentException("Transcrito não encontrado");
        }

        String dna = FileLoader.findFastaSequence(id);
        if (dna == null) {
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

        Translator translator = new Translator();

        ProteinResult resultado = translator.process(transcript, mutation);

        if (resultado == null) {
            throw new IllegalStateException("Não foi possível processar a mutação");
        }

        return resultado	;
    }
}
