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

 public static final int procedureResultUnknown = 0;
 public static final int procedureNoResult = 1;
 public static final int procedureReturnsResult = 2;

 public static final int procedureColumnUnknown = 0;
 public static final int procedureColumnIn = 1;
 public static final int procedureColumnInOut = 2;
 public static final int procedureColumnOut = 4;
 public static final int procedureColumnReturn = 5;
 public static final int procedureColumnResult = 3;

 public static final int procedureNoNulls = 0;
 public static final int procedureNullable = 1;
 public static final int procedureNullableUnknown = 2;

 public static final int columnNoNulls = 0;
 public static final int columnNullable = 1;
 public static final int columnNullableUnknown = 2;

 public static final int bestRowTemporary = 0;
 public static final int bestRowTransaction = 1;
 public static final int bestRowSession = 2;

 public static final int bestRowUnknown = 0;
 public static final int bestRowNotPseudo = 1;
 public static final int bestRowPseudo = 2;

 public static final int versionColumnUnknown = 0;
 public static final int versionColumnNotPseudo = 1;
 public static final int versionColumnPseudo = 2;

 public static final int importedKeyCascade = 0;
 public static final int importedKeyRestrict = 1;
 public static final int importedKeySetNull = 2;
 public static final int importedKeyNoAction = 3;
 public static final int importedKeySetDefault = 4;
 public static final int importedKeyInitiallyDeferred = 5;
 public static final int importedKeyInitiallyImmediate = 6;
 public static final int importedKeyNotDeferrable = 7;

 public static final int typeNoNulls = 0;
 public static final int typeNullable = 1;
 public static final int typeNullableUnknown = 2;
 public static final int typePredNone = 0;
 public static final int typePredChar = 1;
 public static final int typePredBasic = 2;
 public static final int typeSearchable = 3;

 public static final short tableIndexStatistic = 0;
 public static final short tableIndexClustered = 1;
 public static final short tableIndexHashed = 2;
 public static final short tableIndexOther = 3;

 public abstract boolean allProceduresAreCallable() throws SQLException;
 public abstract boolean allTablesAreSelectable() throws SQLException;
 public abstract String getURL() throws SQLException;
 public abstract String getUserName() throws SQLException;
 public abstract boolean isReadOnly() throws SQLException;
 public abstract boolean nullsAreSortedHigh() throws SQLException;
 public abstract boolean nullsAreSortedLow() throws SQLException;
 public abstract boolean nullsAreSortedAtStart() throws SQLException;
 public abstract boolean nullsAreSortedAtEnd() throws SQLException;
 public abstract String getDatabaseProductName() throws SQLException;
 public abstract String getDatabaseProductVersion() throws SQLException;
 public abstract String getDriverName() throws SQLException;
 public abstract String getDriverVersion() throws SQLException;
 public abstract int getDriverMajorVersion();
 public abstract int getDriverMinorVersion();
 public abstract boolean usesLocalFiles() throws SQLException;
 public abstract boolean usesLocalFilePerTable() throws SQLException;
 public abstract boolean supportsMixedCaseIdentifiers() throws SQLException;
 public abstract boolean storesUpperCaseIdentifiers() throws SQLException;
 public abstract boolean storesLowerCaseIdentifiers() throws SQLException;
 public abstract boolean storesMixedCaseIdentifiers() throws SQLException;
 public abstract boolean supportsMixedCaseQuotedIdentifiers() throws SQLException;
 public abstract boolean storesUpperCaseQuotedIdentifiers() throws SQLException;
 public abstract boolean storesLowerCaseQuotedIdentifiers() throws SQLException;
 public abstract boolean storesMixedCaseQuotedIdentifiers() throws SQLException;
 public abstract String getIdentifierQuoteString() throws SQLException;
 public abstract String getSQLKeywords() throws SQLException;
 public abstract String getNumericFunctions() throws SQLException;
 public abstract String getStringFunctions() throws SQLException;
 public abstract String getSystemFunctions() throws SQLException;
 public abstract String getTimeDateFunctions() throws SQLException;
 public abstract String getSearchStringEscape() throws SQLException;
 public abstract String getExtraNameCharacters() throws SQLException;
 public abstract boolean supportsAlterTableWithAddColumn() throws SQLException;
 public abstract boolean supportsAlterTableWithDropColumn() throws SQLException;
 public abstract boolean supportsColumnAliasing() throws SQLException;
 public abstract boolean nullPlusNonNullIsNull() throws SQLException;
 public abstract boolean supportsConvert() throws SQLException;
 public abstract boolean supportsConvert(int fromType, int toType) throws SQLException;
 public abstract boolean supportsTableCorrelationNames() throws SQLException;
 public abstract boolean supportsDifferentTableCorrelationNames() throws SQLException;
 public abstract boolean supportsExpressionsInOrderBy() throws SQLException;
 public abstract boolean supportsOrderByUnrelated() throws SQLException;
 public abstract boolean supportsGroupBy() throws SQLException;
 public abstract boolean supportsGroupByUnrelated() throws SQLException;
 public abstract boolean supportsGroupByBeyondSelect() throws SQLException;
 public abstract boolean supportsLikeEscapeClause() throws SQLException;
 public abstract boolean supportsMultipleResultSets() throws SQLException;
 public abstract boolean supportsMultipleTransactions() throws SQLException;
 public abstract boolean supportsNonNullableColumns() throws SQLException;
 public abstract boolean supportsMinimumSQLGrammar() throws SQLException;
 public abstract boolean supportsCoreSQLGrammar() throws SQLException;
 public abstract boolean supportsExtendedSQLGrammar() throws SQLException;
 public abstract boolean supportsANSI92EntryLevelSQL() throws SQLException;
 public abstract boolean supportsANSI92IntermediateSQL() throws SQLException;
 public abstract boolean supportsANSI92FullSQL() throws SQLException;
 public abstract boolean supportsIntegrityEnhancementFacility() throws SQLException;
 public abstract boolean supportsOuterJoins() throws SQLException;
 public abstract boolean supportsFullOuterJoins() throws SQLException;
 public abstract boolean supportsLimitedOuterJoins() throws SQLException;
 public abstract String getSchemaTerm() throws SQLException;
 public abstract String getProcedureTerm() throws SQLException;
 public abstract String getCatalogTerm() throws SQLException;
 public abstract boolean isCatalogAtStart() throws SQLException;
 public abstract String getCatalogSeparator() throws SQLException;
 public abstract boolean supportsSchemasInDataManipulation() throws SQLException;
 public abstract boolean supportsSchemasInProcedureCalls() throws SQLException;
 public abstract boolean supportsSchemasInTableDefinitions() throws SQLException;
 public abstract boolean supportsSchemasInIndexDefinitions() throws SQLException;
 public abstract boolean supportsSchemasInPrivilegeDefinitions() throws SQLException;
 public abstract boolean supportsCatalogsInDataManipulation() throws SQLException;
 public abstract boolean supportsCatalogsInProcedureCalls() throws SQLException;
 public abstract boolean supportsCatalogsInTableDefinitions() throws SQLException;
 public abstract boolean supportsCatalogsInIndexDefinitions() throws SQLException;
 public abstract boolean supportsCatalogsInPrivilegeDefinitions() throws SQLException;
 public abstract boolean supportsPositionedDelete() throws SQLException;
 public abstract boolean supportsPositionedUpdate() throws SQLException;
 public abstract boolean supportsSelectForUpdate() throws SQLException;
 public abstract boolean supportsStoredProcedures() throws SQLException;
 public abstract boolean supportsSubqueriesInComparisons() throws SQLException;
 public abstract boolean supportsSubqueriesInExists() throws SQLException;
 public abstract boolean supportsSubqueriesInIns() throws SQLException;
 public abstract boolean supportsSubqueriesInQuantifieds() throws SQLException;
 public abstract boolean supportsCorrelatedSubqueries() throws SQLException;
 public abstract boolean supportsUnion() throws SQLException;
 public abstract boolean supportsUnionAll() throws SQLException;
 public abstract boolean supportsOpenCursorsAcrossCommit() throws SQLException;
 public abstract boolean supportsOpenCursorsAcrossRollback() throws SQLException;
 public abstract boolean supportsOpenStatementsAcrossCommit() throws SQLException;
 public abstract boolean supportsOpenStatementsAcrossRollback() throws SQLException;
 public abstract int getMaxBinaryLiteralLength() throws SQLException;
 public abstract int getMaxCharLiteralLength() throws SQLException;
 public abstract int getMaxColumnNameLength() throws SQLException;
 public abstract int getMaxColumnsInGroupBy() throws SQLException;
 public abstract int getMaxColumnsInIndex() throws SQLException;
 public abstract int getMaxColumnsInOrderBy() throws SQLException;
 public abstract int getMaxColumnsInSelect() throws SQLException;
 public abstract int getMaxColumnsInTable() throws SQLException;
 public abstract int getMaxConnections() throws SQLException;
 public abstract int getMaxCursorNameLength() throws SQLException;
 public abstract int getMaxIndexLength() throws SQLException;
 public abstract int getMaxSchemaNameLength() throws SQLException;
 public abstract int getMaxProcedureNameLength() throws SQLException;
 public abstract int getMaxCatalogNameLength() throws SQLException;
 public abstract int getMaxRowSize() throws SQLException;
 public abstract boolean doesMaxRowSizeIncludeBlobs() throws SQLException;
 public abstract int getMaxStatementLength() throws SQLException;
 public abstract int getMaxStatements() throws SQLException;
 public abstract int getMaxTableNameLength() throws SQLException;
 public abstract int getMaxTablesInSelect() throws SQLException;
 public abstract int getMaxUserNameLength() throws SQLException;
 public abstract int getDefaultTransactionIsolation() throws SQLException;
 public abstract boolean supportsTransactions() throws SQLException;
 public abstract boolean supportsTransactionIsolationLevel(int level) throws SQLException;
 public abstract boolean supportsDataDefinitionAndDataManipulationTransactions() throws SQLException;
 public abstract boolean supportsDataManipulationTransactionsOnly() throws SQLException;
 public abstract boolean dataDefinitionCausesTransactionCommit() throws SQLException;
 public abstract boolean dataDefinitionIgnoredInTransactions() throws SQLException;
 public abstract ResultSet getProcedures(String catalog, String schemaPattern, String procedureNamePattern) throws SQLException;
 public abstract ResultSet getProcedureColumns(String catalog, String schemaPattern, String procedureNamePattern, String columnNamePattern) throws SQLException;
 public abstract ResultSet getTables(String catalog, String schemaPattern, String tableNamePattern, String types[]) throws SQLException;
 public abstract ResultSet getSchemas() throws SQLException;
 public abstract ResultSet getCatalogs() throws SQLException;
 public abstract ResultSet getTableTypes() throws SQLException;
 public abstract ResultSet getColumns(String catalog, String schemaPattern, String tableNamePattern, String columnNamePattern) throws SQLException;
 public abstract ResultSet getColumnPrivileges(String catalog, String schema, String table, String columnNamePattern) throws SQLException;
 public abstract ResultSet getTablePrivileges(String catalog, String schemaPattern, String tableNamePattern) throws SQLException;
 public abstract ResultSet getBestRowIdentifier(String catalog, String schema, String table, int scope, boolean nullable) throws SQLException;
 public abstract ResultSet getVersionColumns(String catalog, String schema, String table) throws SQLException;
 public abstract ResultSet getPrimaryKeys(String catalog, String schema, String table) throws SQLException;
 public abstract ResultSet getImportedKeys(String catalog, String schema, String table) throws SQLException;
 public abstract ResultSet getExportedKeys(String catalog, String schema, String table) throws SQLException;
 public abstract ResultSet getCrossReference(String primaryCatalog, String primarySchema, String primaryTable, String foreignCatalog, String foreignSchema, String foreignTable) throws SQLException;
 public abstract ResultSet getTypeInfo() throws SQLException;
 public abstract ResultSet getIndexInfo(String catalog, String schema, String table, boolean unique, boolean approximate) throws SQLException;

}
