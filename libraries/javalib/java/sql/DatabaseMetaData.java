/*
 * Java core library component.
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.sql;

public interface DatabaseMetaData {

 int procedureResultUnknown = 0;
 int procedureNoResult = 1;
 int procedureReturnsResult = 2;

 int procedureColumnUnknown = 0;
 int procedureColumnIn = 1;
 int procedureColumnInOut = 2;
 int procedureColumnOut = 4;
 int procedureColumnReturn = 5;
 int procedureColumnResult = 3;

 int procedureNoNulls = 0;
 int procedureNullable = 1;
 int procedureNullableUnknown = 2;

 int columnNoNulls = 0;
 int columnNullable = 1;
 int columnNullableUnknown = 2;

 int bestRowTemporary = 0;
 int bestRowTransaction = 1;
 int bestRowSession = 2;

 int bestRowUnknown = 0;
 int bestRowNotPseudo = 1;
 int bestRowPseudo = 2;

 int versionColumnUnknown = 0;
 int versionColumnNotPseudo = 1;
 int versionColumnPseudo = 2;

 int importedKeyCascade = 0;
 int importedKeyRestrict = 1;
 int importedKeySetNull = 2;
 int importedKeyNoAction = 3;
 int importedKeySetDefault = 4;
 int importedKeyInitiallyDeferred = 5;
 int importedKeyInitiallyImmediate = 6;
 int importedKeyNotDeferrable = 7;

 int typeNoNulls = 0;
 int typeNullable = 1;
 int typeNullableUnknown = 2;
 int typePredNone = 0;
 int typePredChar = 1;
 int typePredBasic = 2;
 int typeSearchable = 3;

 short tableIndexStatistic = 0;
 short tableIndexClustered = 1;
 short tableIndexHashed = 2;
 short tableIndexOther = 3;

 boolean allProceduresAreCallable() throws SQLException;
 boolean allTablesAreSelectable() throws SQLException;
 String getURL() throws SQLException;
 String getUserName() throws SQLException;
 boolean isReadOnly() throws SQLException;
 boolean nullsAreSortedHigh() throws SQLException;
 boolean nullsAreSortedLow() throws SQLException;
 boolean nullsAreSortedAtStart() throws SQLException;
 boolean nullsAreSortedAtEnd() throws SQLException;
 String getDatabaseProductName() throws SQLException;
 String getDatabaseProductVersion() throws SQLException;
 String getDriverName() throws SQLException;
 String getDriverVersion() throws SQLException;
 int getDriverMajorVersion();
 int getDriverMinorVersion();
 boolean usesLocalFiles() throws SQLException;
 boolean usesLocalFilePerTable() throws SQLException;
 boolean supportsMixedCaseIdentifiers() throws SQLException;
 boolean storesUpperCaseIdentifiers() throws SQLException;
 boolean storesLowerCaseIdentifiers() throws SQLException;
 boolean storesMixedCaseIdentifiers() throws SQLException;
 boolean supportsMixedCaseQuotedIdentifiers() throws SQLException;
 boolean storesUpperCaseQuotedIdentifiers() throws SQLException;
 boolean storesLowerCaseQuotedIdentifiers() throws SQLException;
 boolean storesMixedCaseQuotedIdentifiers() throws SQLException;
 String getIdentifierQuoteString() throws SQLException;
 String getSQLKeywords() throws SQLException;
 String getNumericFunctions() throws SQLException;
 String getStringFunctions() throws SQLException;
 String getSystemFunctions() throws SQLException;
 String getTimeDateFunctions() throws SQLException;
 String getSearchStringEscape() throws SQLException;
 String getExtraNameCharacters() throws SQLException;
 boolean supportsAlterTableWithAddColumn() throws SQLException;
 boolean supportsAlterTableWithDropColumn() throws SQLException;
 boolean supportsColumnAliasing() throws SQLException;
 boolean nullPlusNonNullIsNull() throws SQLException;
 boolean supportsConvert() throws SQLException;
 boolean supportsConvert(int fromType, int toType) throws SQLException;
 boolean supportsTableCorrelationNames() throws SQLException;
 boolean supportsDifferentTableCorrelationNames() throws SQLException;
 boolean supportsExpressionsInOrderBy() throws SQLException;
 boolean supportsOrderByUnrelated() throws SQLException;
 boolean supportsGroupBy() throws SQLException;
 boolean supportsGroupByUnrelated() throws SQLException;
 boolean supportsGroupByBeyondSelect() throws SQLException;
 boolean supportsLikeEscapeClause() throws SQLException;
 boolean supportsMultipleResultSets() throws SQLException;
 boolean supportsMultipleTransactions() throws SQLException;
 boolean supportsNonNullableColumns() throws SQLException;
 boolean supportsMinimumSQLGrammar() throws SQLException;
 boolean supportsCoreSQLGrammar() throws SQLException;
 boolean supportsExtendedSQLGrammar() throws SQLException;
 boolean supportsANSI92EntryLevelSQL() throws SQLException;
 boolean supportsANSI92IntermediateSQL() throws SQLException;
 boolean supportsANSI92FullSQL() throws SQLException;
 boolean supportsIntegrityEnhancementFacility() throws SQLException;
 boolean supportsOuterJoins() throws SQLException;
 boolean supportsFullOuterJoins() throws SQLException;
 boolean supportsLimitedOuterJoins() throws SQLException;
 String getSchemaTerm() throws SQLException;
 String getProcedureTerm() throws SQLException;
 String getCatalogTerm() throws SQLException;
 boolean isCatalogAtStart() throws SQLException;
 String getCatalogSeparator() throws SQLException;
 boolean supportsSchemasInDataManipulation() throws SQLException;
 boolean supportsSchemasInProcedureCalls() throws SQLException;
 boolean supportsSchemasInTableDefinitions() throws SQLException;
 boolean supportsSchemasInIndexDefinitions() throws SQLException;
 boolean supportsSchemasInPrivilegeDefinitions() throws SQLException;
 boolean supportsCatalogsInDataManipulation() throws SQLException;
 boolean supportsCatalogsInProcedureCalls() throws SQLException;
 boolean supportsCatalogsInTableDefinitions() throws SQLException;
 boolean supportsCatalogsInIndexDefinitions() throws SQLException;
 boolean supportsCatalogsInPrivilegeDefinitions() throws SQLException;
 boolean supportsPositionedDelete() throws SQLException;
 boolean supportsPositionedUpdate() throws SQLException;
 boolean supportsSelectForUpdate() throws SQLException;
 boolean supportsStoredProcedures() throws SQLException;
 boolean supportsSubqueriesInComparisons() throws SQLException;
 boolean supportsSubqueriesInExists() throws SQLException;
 boolean supportsSubqueriesInIns() throws SQLException;
 boolean supportsSubqueriesInQuantifieds() throws SQLException;
 boolean supportsCorrelatedSubqueries() throws SQLException;
 boolean supportsUnion() throws SQLException;
 boolean supportsUnionAll() throws SQLException;
 boolean supportsOpenCursorsAcrossCommit() throws SQLException;
 boolean supportsOpenCursorsAcrossRollback() throws SQLException;
 boolean supportsOpenStatementsAcrossCommit() throws SQLException;
 boolean supportsOpenStatementsAcrossRollback() throws SQLException;
 int getMaxBinaryLiteralLength() throws SQLException;
 int getMaxCharLiteralLength() throws SQLException;
 int getMaxColumnNameLength() throws SQLException;
 int getMaxColumnsInGroupBy() throws SQLException;
 int getMaxColumnsInIndex() throws SQLException;
 int getMaxColumnsInOrderBy() throws SQLException;
 int getMaxColumnsInSelect() throws SQLException;
 int getMaxColumnsInTable() throws SQLException;
 int getMaxConnections() throws SQLException;
 int getMaxCursorNameLength() throws SQLException;
 int getMaxIndexLength() throws SQLException;
 int getMaxSchemaNameLength() throws SQLException;
 int getMaxProcedureNameLength() throws SQLException;
 int getMaxCatalogNameLength() throws SQLException;
 int getMaxRowSize() throws SQLException;
 boolean doesMaxRowSizeIncludeBlobs() throws SQLException;
 int getMaxStatementLength() throws SQLException;
 int getMaxStatements() throws SQLException;
 int getMaxTableNameLength() throws SQLException;
 int getMaxTablesInSelect() throws SQLException;
 int getMaxUserNameLength() throws SQLException;
 int getDefaultTransactionIsolation() throws SQLException;
 boolean supportsTransactions() throws SQLException;
 boolean supportsTransactionIsolationLevel(int level) throws SQLException;
 boolean supportsDataDefinitionAndDataManipulationTransactions() throws SQLException;
 boolean supportsDataManipulationTransactionsOnly() throws SQLException;
 boolean dataDefinitionCausesTransactionCommit() throws SQLException;
 boolean dataDefinitionIgnoredInTransactions() throws SQLException;
 ResultSet getProcedures(String catalog, String schemaPattern, String procedureNamePattern) throws SQLException;
 ResultSet getProcedureColumns(String catalog, String schemaPattern, String procedureNamePattern, String columnNamePattern) throws SQLException;
 ResultSet getTables(String catalog, String schemaPattern, String tableNamePattern, String types[]) throws SQLException;
 ResultSet getSchemas() throws SQLException;
 ResultSet getCatalogs() throws SQLException;
 ResultSet getTableTypes() throws SQLException;
 ResultSet getColumns(String catalog, String schemaPattern, String tableNamePattern, String columnNamePattern) throws SQLException;
 ResultSet getColumnPrivileges(String catalog, String schema, String table, String columnNamePattern) throws SQLException;
 ResultSet getTablePrivileges(String catalog, String schemaPattern, String tableNamePattern) throws SQLException;
 ResultSet getBestRowIdentifier(String catalog, String schema, String table, int scope, boolean nullable) throws SQLException;
 ResultSet getVersionColumns(String catalog, String schema, String table) throws SQLException;
 ResultSet getPrimaryKeys(String catalog, String schema, String table) throws SQLException;
 ResultSet getImportedKeys(String catalog, String schema, String table) throws SQLException;
 ResultSet getExportedKeys(String catalog, String schema, String table) throws SQLException;
 ResultSet getCrossReference(String primaryCatalog, String primarySchema, String primaryTable, String foreignCatalog, String foreignSchema, String foreignTable) throws SQLException;
 ResultSet getTypeInfo() throws SQLException;
 ResultSet getIndexInfo(String catalog, String schema, String table, boolean unique, boolean approximate) throws SQLException;

}
