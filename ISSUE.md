# Nuova sessione token-efficient per lavorare sulle issue

Lavora in modo token-efficient e segui `AGENTS.md`.

Non usare `/resume`.
Non ricostruire vecchie sessioni.
Usa solo lo stato attuale della repository e il contesto minimo necessario.

## Lettura iniziale consentita

Prima di proporre modifiche, leggi solo:

1. `AGENTS.md`
2. `git status --short`
3. `git diff --stat`
4. `tail -30 SESSION.md`, se esiste
5. lista compatta delle issue aperte, senza body e senza commenti

Comandi consigliati:

```bash
timeout 30s git status --short
timeout 30s git diff --stat
timeout 30s tail -30 SESSION.md
timeout 30s gh issue list --limit 20 --json number,title,state,labels
```

## Regole di contesto

* Non leggere tutta la repository.
* Non stampare file interi.
* Non usare `cat` su file grandi.
* Non eseguire `rg .`, `find .`, `git log` o comandi ampi senza limiti.
* Non scaricare issue GitHub in blocco con body o commenti.
* Non eseguire test completi finché non abbiamo deciso il perimetro.
* Non usare output superiori a 120 righe.
* Non modificare file prima del mio `OK`.

## Task corrente

Stiamo lavorando sulle issue GitHub di questa repository.

Obiettivo iniziale:

1. mostrare una lista compatta delle issue aperte;
2. raggrupparle in piccoli batch coerenti;
3. consigliare quale issue o batch affrontare per primo;
4. aspettare la mia scelta prima di leggere body, commenti o file specifici.

## Output richiesto ora

Rispondi solo con:

1. stato Git sintetico;
2. lista compatta delle issue aperte: numero, titolo, labels;
3. proposta di batch piccoli e coerenti;
4. quale batch/issue consiglieresti di affrontare per primo e perché;
5. quali file leggeresti solo dopo la mia approvazione.

Aspetta il mio `OK` prima di aprire altri file o modificare codice.
