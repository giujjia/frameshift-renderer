package model;

import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class HGVSParser {
    //valida entrada recebida pela mainwindow
	
    public static Mutation parse(String hgvs){
        
        //Primeira validação: a entrada começa com "c."?
        if( hgvs != null && hgvs.startsWith("c.")){
            
            //remove o c. para facilitar operações
            hgvs = hgvs.substring(2);
            //verifica se é delins
            Pattern RE_DELINS = Pattern.compile("^(\\d+)_(\\d+)del([ACGTN]+)ins([ACGTN]+)$",Pattern.CASE_INSENSITIVE);
            //verifica se é del simples (uma base)
            Pattern RE_DEL_SINGLE = Pattern.compile("^(\\d+)del([ACGTN]+)?$",Pattern.CASE_INSENSITIVE);
            //verfiica se é del de intervalo
            Pattern RE_DEL_RANGE = Pattern.compile("^(\\d+)_(\\d+)del([ACGTN]+)?$",Pattern.CASE_INSENSITIVE);
            //verifica se é inserção
            Pattern RE_INS = Pattern.compile("^(\\d+)_(\\d+)ins([ACGTN]+)$",Pattern.CASE_INSENSITIVE);

            Matcher m;
            m = RE_DELINS.matcher(hgvs);
            if (m.matches()) {
                //armazena os dados da operação
                String del_seq = m.group(3).toUpperCase();
                String ins_seq = m.group(4).toUpperCase();
                //Validação de intervalo: separa o regex em grupos - Se início do intervalo for menor do que o fim, retorna null
                int pos1 = Integer.parseInt(m.group(1));
                int pos2 = Integer.parseInt(m.group(2));
                if (pos1>pos2) {
                    return null;
                }
                return new Mutation(pos1, del_seq.length(), ins_seq, MutationType.DELINS);
                
            }
            //DELEÇÃO SINGULAR
            else if ((m = RE_DEL_SINGLE.matcher(hgvs)).matches()) {
                String del_seq = m.group(2);

                int pos1 = Integer.parseInt(m.group(1));

                int delLen;

                if(del_seq != null){
                    del_seq = del_seq.toUpperCase();
                    delLen = del_seq.length();
                }else{
                    delLen=1;
                }
                
                //porção insertSeq do construtor vazia pois a operação é de deleção, logo nã há inserção.
                return new Mutation(pos1, del_seq.length(),"", MutationType.DELETION);
            }
            //DELEÇÃO MULTIPLA
            else if ((m = RE_DEL_RANGE.matcher(hgvs)).matches()) {
                //essa parte é diferente das outras funções porque: Caso o grupo 3 não seja providenciado
                //o retorno da string será null. Logo é necessário separar o toUpperCase pois null.toUpperCase crasha o programa
                String del_seq = m.group(3);
                
                int pos1 = Integer.parseInt(m.group(1));
                int pos2 = Integer.parseInt(m.group(2));
                if (pos1>pos2) {
                    return null;
                }
                int delLen;
                //em notação HGVS nem sempre o que está sendo deletado exatamente precisa ser identificado.
                //logo, surge a necessidade de ver quantas "casas" estão sendo deletadas
                //SE o que deve ser deletado não for especificado
                //posição final - posição inicial + 1 = número de casas a serem apagadas
                //125_128del: 128-125 = 3+1, logo delLenght é 4
                //
                if(del_seq!=null){
                    del_seq = del_seq.toUpperCase();
                    delLen = del_seq.length();
                } else{
                    delLen = pos2 - pos1 + 1;
                }
                return new Mutation(pos1, delLen,"",MutationType.DELETION);
            }
            //INSERÇÃO SIMPLES
            else if ((m = RE_INS.matcher(hgvs)).matches()) {
                String ins_seq = m.group(3).toUpperCase();

                int pos1 = Integer.parseInt(m.group(1));
                int pos2 = Integer.parseInt(m.group(2));
                if (pos1>pos2) {
                    return null;
                }
                return new Mutation(pos1, 0, ins_seq, MutationType.INSERTION);
            }
                }
        return null;
    }
}
